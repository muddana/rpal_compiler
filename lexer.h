//class for lexer. All the functions with handle* deal with indentifying a token. 

class RpalLexer{
public:

  RpalLexer(ifstream *fileHandle);  
  const Token *nextToken();

private:

  //handles End Of File 
  void handleEOF(){
    if(invalidState())
      return;

    if(isEOF(fileHndl->peek())){
      buildManageTokens(Token::ENDOFFILE, "EOF");
    }
  };

  //handles operators
  //Operator -> Operator_symbol+ => ’<OPERATOR>’;
  //Operator_symbol
  //         -> ’+’ | ’-’ | ’*’ | ’<’ | ’>’ | ’&’ | ’.’
  //          | ’@’ | ’/’ | ’:’ | ’=’ | ’~’ | ’|’ | ’$’
  //          | ’!’ | ’#’ | ’%’ | ’ˆ’ | ’_’ | ’[’ | ’]’
  //          | ’{’ | ’}’ | ’"’ | ’‘’ | ’?’;
  //vector<char> operators(_ops,_ops +  sizeof(_ops)/sizeof(char));
  void handleOperators(){
    if(invalidState())
      return;

    if(isOperatorSymbol(fileHndl->peek())){
      do{
	readNextCharToStack();
      }while(isOperatorSymbol(fileHndl->peek()));
      string tokData = extractTokData();
      buildManageTokens(Token::OPERATOR, tokData);
    }
  };
    
  //handles punctions
  //Punction -> ’(’ => ’(’
  //       -> ’)’ => ’)’
  //       -> ’;’ => ’;’
  //       -> ’,’ => ’,’;
  void handlePunction(){
    if(invalidState())
      return;
	
    char nextChar = fileHndl->peek();
    if( isPunction(nextChar) )
      {
	readNextCharToStack();
	string tokData = extractTokData();
	buildManageTokens(Token::PUNCTION, tokData);
      }
  };

  //handles strings
  //String -> ’’’’
  //        ( ’\’ ’t’ | ’\’ ’n’ | ’\’ ’\’ | ’\’ ’’’’
  //        | ’(’     | ’)’     | ’;’     | ’,’
  //        | ’ ’
  //        | Letter | Digit | Operator_symbol
  //        )* ’’’’                                  => ’<STRING>’;

  //ascii 39 for \' (single quote)
  void handleStrings(){
    if(invalidState())
      return;
    
    char readChar;
    if(fileHndl->peek() == 39){
      do{
	//cout << "Read char is : " << fileHndl->peek() << endl;
	if(fileHndl->peek() != '\\'){//(read_char != '\''){
	  readNextCharToStack();
	  readChar = fileHndl->peek();
	}
	else{
	  getEscapeSequence();
	}
      }while(fileHndl->peek() == 32 || isDigit(fileHndl->peek()) || isLetter(fileHndl->peek()) || isOperatorSymbol(fileHndl->peek()) || isPunction(fileHndl->peek()) || fileHndl->peek() == '\\');//read_char == 32 || isDigit(read_char) || isLetter(read_char) || isOperatorSymbol(read_char) || isPunction(read_char) || read_char=='\\');
      //ascii 32 stands for space
      //upon leaving the loop the next token should be end of string(single quote) 
	//cout << "coming out of the get string loop, fileHndl->peek() : " << fileHndl->peek() << " and read_char : "  << read_char  << endl ;
	if(fileHndl->peek() == 39){
	readNextCharToStack();
	string tokData = extractTokData();
        buildManageTokens(Token::STRING, tokData);
      }
      else{ 
	throw RpalError(RpalError::LEXER, "Expected ' but found " + itos(fileHndl->peek()));
      };
    };
  };

  void handleComments(){
    if(invalidState())
      return;
    
    char readChar = fileHndl->peek();
    if(readChar == '/'){
      readNextCharToStack();
      if(fileHndl->peek() == '/'){
	readNextCharToStack();
	while(isCommentCharacter(fileHndl->peek())){
	  readNextCharToStack();
	}
	//after the loop check for end of line character else exception
	// ascii 10 
	if(fileHndl->peek() == '\n') 
	  {
	    readNextCharToStack();
	    string tokData = extractTokData();
	    buildManageTokens(Token::DELETE, tokData);
	  }
	else{
	  throw RpalError(RpalError::LEXER, "End of line character for comment not found");
	}
      }
      else{
	fileHndl->putback(readChar);//read_char has to be always '/' 
	//i think i have to empty the stack there case here is if the  has a single '/'
	charStack.pop();
      }
    }
  };

  //handles integers
  //Integer -> Digit+ => ’<INTEGER>’;
  void handleIntegers(){
    if(invalidState())
      return;

    if(isDigit(fileHndl->peek()))
      {
	do{
	  readNextCharToStack();
	}while(isDigit(fileHndl->peek()));
	string tokData = extractTokData();
	buildManageTokens(Token::INTEGER, tokData);
      }
  };
  //handles whitespaces, Eof and tab spaces
  //Spaces -> ( ’ ’ | ht | Eol )+ => ’<DELETE>’;
  void handleSpaces(){
    if(invalidState())
      return;

    if (isWhiteSpaceOrEOL(fileHndl->peek())){
    do{
      readNextCharToStack();
    }while(isWhiteSpaceOrEOL(fileHndl->peek()));
    string tokData = extractTokData();
    buildManageTokens(Token::DELETE, tokData);
    };
  };
  //handles identifiers
  //Identifier -> Letter (Letter | Digit | ’_’)* => ’<IDENTIFIER>’;
  void handleIdentifier()
  {
    if(invalidState())
      return;

    if(isLetter(fileHndl->peek()))
      {
	do{
	  readNextCharToStack();
	}
	while(isLetter(fileHndl->peek()) || isDigit(fileHndl->peek()) || fileHndl->peek() == '_');
	//empty the stack build a token and manage it
	string tokData =  extractTokData();
	buildManageTokens(Token::IDENTIFIER, tokData);
      }
    
  };

  
  void getEscapeSequence(){
    if(fileHndl->peek() == '\\'){
      readNextCharToStack();
      char read_char = fileHndl->peek();
      if(read_char == 't' || read_char == 'n' || read_char == '\\' || read_char == '\''){
	readNextCharToStack();
      }
      else{
	throw RpalError(RpalError::LEXER, "illegal token: illegal escape character" + itos(fileHndl->peek()));
      }
    }
  };

  bool invalidState() const{
    if(TOKENFOUND == currState)
      return true;
    else
      return false;
  };

  void setup(){
    initOperators();
    initTokens();
  };  

  void readNextCharToStack(){
    char temp[1];
    fileHndl->read(temp, 1);
    charStack.push(*temp);
  };

  //need to pass the type of identifier to be passed to buildManageTokens
  string extractTokData(){
    string tokData = "";
    while(!charStack.empty())
      {
	tokData.push_back(charStack.top());
	charStack.pop();
      }
    reverse(tokData.begin(), tokData.end());
    return tokData;
  };

  void buildManageTokens(int token_type, string temp){
    //cout << "building tok, type: " << itos(token_type) << " value: " << temp << endl;
    currState = TOKENFOUND;
    prev_tok = curr_tok;
    curr_tok = new Token(token_type, temp);
  };

  bool isEOF(int c) const{
    if(c == EOF)
      return true;
    else
      return false;
  };
  bool isLetter(int c) const{
    if((c >= 65 && c <= 90) || (c >= 97 && c<= 122) )
	return true;
    else
      return false;
  };

  bool isDigit(int c) const{
    if( c >= 48 && c <= 57)
      return true;
    else
      return false;
  };

  bool isWhiteSpaceOrEOL(char c) const{
    //9 for horizontal tab, 32 space, 10 for Line feed
    if( 9 == c ||  32 == c || 10 == c)
      return true;
    else
      return false;
  };

  void initOperators(){
    string op_str = "+,-,*,<,>,&,.,@,/,:,=,~,|,$,!,#,%,^,_,[,],{,},\",`,?";
    tokenize(op_str, ',', &operator_list);
  };
  
  void initTokens(){
    prev_tok = NULL;
    curr_tok = NULL;
  };

  //TODO not sure why the == for two strings and also the find on operator_list not working
  //inline functions
  inline bool isOperatorSymbol(int next_char) const{
    int i=0;
    bool is_operator = false;
    while(i < operator_list.size()){
      //why can't i store a vector of strings ?
      if(next_char == operator_list.at(i).at(0)){
	is_operator = true;
	break;
      };
      i = i+1;
    };
    return is_operator;
  };
  
  inline bool isPunction(char next_char) const{
    if('(' == next_char || ')' == next_char || ';' == next_char || ',' == next_char)
      return true;
    else
      return false;
  };

  inline bool isCommentCharacter(char chr){
    if('\'' == chr || isPunction(chr) || '\\' == chr || ' ' == chr || '\t' == chr || isLetter(chr) ||  isDigit(chr) || isOperatorSymbol(chr))
      return true;
      else
	return false;
  };

  void getNextToken(){
    currState = SEARCHING;
      handleEOF();
      handleIdentifier();
      handleIntegers();
      handleComments();
      handleOperators();
      handleStrings();
      handleSpaces();
      handlePunction();
  };
  const Token *getScreenedNextToken(){
    do{
      getNextToken();
    }while(!is_useful_token(curr_tok));
    return curr_tok;
  };
  bool is_useful_token(const Token *tok) const{
    if(Token::DELETE == tok->type()){
      return false;
    }
    else
      return true;
  };

  //members
  enum States {TOKENFOUND = 0, SEARCHING = 1};
  States currState;
  const Token *curr_tok, *prev_tok, *next_tok;
  vector<string> progContent;//fileContent
  ifstream * fileHndl;
  stack<char> charStack;//to store the input characters while making decisions
  vector<string> operator_list;
  char _ops[];
};

