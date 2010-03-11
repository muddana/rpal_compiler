//
// C++ Implementation: test
//
// Description: Lexical Analyzer for rpal the handle*() functions (eg. handlePunction()) handle looking for the tokens. getScreenedNextToken() acts as a screener which doesn't pass the tokens to the parser
//
//
// Author: srinivas muddana, <smuddana@cise.ufl.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "helper.h"

class Token{
public:
  Token(): _value(""), _type(0){
  };
  Token(string token_value):_value(token_value), _type(0){
  };
  Token(int type_of_tok): _value(""),_type(type_of_tok){
  };
  Token(int type_of_tok, string token_value): _value(token_value),_type(type_of_tok){
  };
  enum Types { IDENTIFIER = 0,
	       INTEGER = 1,
	       OPERATOR = 2,
               STRING =  3,
	       DELETE = 4,
	       PUNCTION = 5,
	       ENDOFFILE = 6
              };
  int type() const{
    return _type;
  };
  string value() const{
    return _value;
  };
private:
  const string _value;
  const int _type;
};

class RpalLexer{
public:
  RpalLexer(){
    setup();
  };
  RpalLexer(ifstream *fileHandle){
    setup();
    fileHndl = fileHandle;
  };
  Token *next_token(){
    return getScreenedNextToken();
  };
private:

  bool invalidState(){
    return false;
  };

  void setup(){
    initOperators();
    initTokens();
  };
  void handleEOF(void(RpalLexer::*handler)(int,std::string)){
    if(invalidState())
      return;

    if(isEOF(fileHndl->peek()))
      (this->*handler)(Token::ENDOFFILE, "EOF");      
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
	cout << endl << "Expected ' but found :" << (char)fileHndl->peek() << endl;
	throw "end of string not found should hit eof here did it ? ";
      };
    };
  };
  
  void getEscapeSequence(){
    if(fileHndl->peek() == '\\'){
      readNextCharToStack();
      char read_char = fileHndl->peek();
      if(read_char == 't' || read_char == 'n' || read_char == '\\' || read_char == '\''){
	readNextCharToStack();
      }
      else{
	throw "illegal token: illegal escape character";
      }
    }
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
	  cout << "End of line character for comment not found";
	  throw "End of line character for comment not found";
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
	prev_tok = curr_tok;
	curr_tok = new Token(token_type, temp);
  };
  bool isEOF(int c)
  {
    if(c == EOF)
      return true;
    else
      return false;
  };
  bool isLetter(int c){
    if((c >= 65 && c <= 90) || (c >= 97 && c<= 122) )
	return true;
    else
      return false;
  };
  bool isDigit(int c){
    if( c >= 48 && c <= 57)
      return true;
    else
      return false;
  };
  bool isWhiteSpaceOrEOL(char c){
    //9 for horizontal tab, 32 space, 10 for Line feed
    if(c == 9 || c == 32 || c == 10)
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
  inline bool isOperatorSymbol(int next_char){
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
  
  inline bool isPunction(char next_char){
    if(next_char == '(' || next_char == ')' || next_char == ';' || next_char == ',')
      return true;
    else
      return false;
  };
  inline bool isCommentCharacter(char chr){
    if(chr == '\'' || isPunction(chr) || chr == '\\' || chr == ' ' || chr == '\t' || isLetter(chr) ||  isDigit(chr) || isOperatorSymbol(chr))
      return true;
      else
	return false;
  };

  void getNextToken(){
    currState = SEARCHING;
    void(RpalLexer::*eofHandler)(int,std::string);
    eofHandler = &RpalLexer::buildManageTokens;
    //need to implement fall though
    try{
      handleEOF(eofHandler);
      handleIdentifier();
      handleIntegers();
      handleComments();
      handleOperators();
      handleStrings();
      handleSpaces();
      handlePunction();
    }
    catch(int ex_num){
      cout << "Lexer Error code :"  << ex_num << endl;
      throw ex_num;
    }
    catch(...){ 
      cout << "UnExpected Exception" << endl;
    }
  };
  Token *getScreenedNextToken(){
    do{
      getNextToken();
    }while(!is_useful_token(curr_tok));
    return curr_tok;
  };
  bool is_useful_token(Token *tok){
    if(tok->type() == Token::DELETE){
      return false;
    }
    else
      return true;
  };
  //members
  enum States {TOKENFOUND = 0, SEARCHING = 1};
  States currState;
  Token *curr_tok, *prev_tok, *next_tok;
  vector<string> progContent;//fileContent
  ifstream *fileHndl;
  stack<char> charStack;//to store the input characters while making decisions
  vector<string> operator_list;
  char _ops[];
};

