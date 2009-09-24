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
  Token(){
  };
  Token(string token_value){
    _value = token_value;
  };
  Token(int type_of_tok){
    _type = type_of_tok;
  };
  Token(int type_of_tok, string token_value){
    _value = token_value;
    _type = type_of_tok;
  };
  enum Types { IDENTIFIER = 0,
	       INTEGER = 1,
	       OPERATOR = 2,
               STRING =  3,
	       DELETE = 4,
	       PUNCTION = 5,
	       ENDOFFILE = 6
              };
  int type(){
    return _type;
  };
  string value(){
    return _value;
  };
  //need to move to private
  string _value;
  int _type;
private:
};

class RpalLexer{
public:
  //static const int MAX_LINE_LENGTH = 1024;
  RpalLexer(){
    init_ops();
    init_tokens();
  };
  RpalLexer(ifstream *fileHandle){
    fileHndl = fileHandle;
    init_ops();
    init_tokens();
  };
  Token *next_token(){
    return getScreenedNextToken();
  };
private:

  void handleEOF(){
    if(isEOF(fileHndl->peek()))
      throw EOF_ENCOUNTERED;
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
    if(isOperatorSymbol(fileHndl->peek())){
      do{
	read_next_char_to_stack();
      }while(isOperatorSymbol(fileHndl->peek()));
      extractToken(Token::OPERATOR);
    }
  };
    
  //handles punctions
  //Punction -> ’(’ => ’(’
  //       -> ’)’ => ’)’
  //       -> ’;’ => ’;’
  //       -> ’,’ => ’,’;
  void handlePunction(){
    char next_char = fileHndl->peek();
    if( isPunction(next_char) )
      {
	read_next_char_to_stack();
	extractToken(Token::PUNCTION);
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
    char read_char;
    if(fileHndl->peek() == 39){
      do{
	//cout << "Read char is : " << fileHndl->peek() << endl;
	if(fileHndl->peek() != '\\'){//(read_char != '\''){
	  read_next_char_to_stack();
	  read_char = fileHndl->peek();
	}
	else{
	  getEscapeSequence();
	}
      }while(fileHndl->peek() == 32 || isDigit(fileHndl->peek()) || isLetter(fileHndl->peek()) || isOperatorSymbol(fileHndl->peek()) || isPunction(fileHndl->peek()) || fileHndl->peek() == '\\');//read_char == 32 || isDigit(read_char) || isLetter(read_char) || isOperatorSymbol(read_char) || isPunction(read_char) || read_char=='\\');
      //ascii 32 stands for space
      //upon leaving the loop the next token should be end of string(single quote) 
	//cout << "coming out of the get string loop, fileHndl->peek() : " << fileHndl->peek() << " and read_char : "  << read_char  << endl ;
	if(fileHndl->peek() == 39){
	read_next_char_to_stack();
	extractToken(Token::STRING);
      }
      else{ 
	cout << endl << "Expected ' but found :" << (char)fileHndl->peek() << endl;
	throw "end of string not found should hit eof here did it ? ";
      };
    };
  };
  
  void getEscapeSequence(){
    if(fileHndl->peek() == '\\'){
      read_next_char_to_stack();
      char read_char = fileHndl->peek();
      if(read_char == 't' || read_char == 'n' || read_char == '\\' || read_char == '\''){
	read_next_char_to_stack();
      }
      else{
	throw "illegal token: illegal escape character";
      }
    }
  };

  void handleComments(){
    char read_char = fileHndl->peek();
    if(read_char == '/'){
      read_next_char_to_stack();
      if(fileHndl->peek() == '/'){
	read_next_char_to_stack();
	while(isCommentCharacter(fileHndl->peek())){
	  read_next_char_to_stack();
	}
	//after the loop check for end of line character else exception
	// ascii 10 
	if(fileHndl->peek() == '\n') 
	  {
	    read_next_char_to_stack();
	    extractToken(Token::DELETE);
	  }
	else{
	  cout << "End of line character for comment not found";
	  throw "End of line character for comment not found";
	}
      }
      else{
	fileHndl->putback(read_char);//read_char has to be always '/' 
	//i think i have to empty the stack there case here is if the  has a single '/'
	charStack.pop();
      }
    }
  };

  //handles integers
  //Integer -> Digit+ => ’<INTEGER>’;
  void handleIntegers(){
    if(isDigit(fileHndl->peek()))
      {
	do{
	  read_next_char_to_stack();
	}while(isDigit(fileHndl->peek()));
	extractToken(Token::INTEGER);
      }
  };
  //handles whitespaces, Eof and tab spaces
  //Spaces -> ( ’ ’ | ht | Eol )+ => ’<DELETE>’;
  void handleSpaces(){
    if (isWhiteSpaceOrEOL(fileHndl->peek())){
    do{
      read_next_char_to_stack();
    }while(isWhiteSpaceOrEOL(fileHndl->peek()));
    extractToken(Token::DELETE);
    };
  };
  //handles identifiers
  //Identifier -> Letter (Letter | Digit | ’_’)* => ’<IDENTIFIER>’;
  void handleIdentifier()
  {
    if(isLetter(fileHndl->peek()))
      {
	do{
	  read_next_char_to_stack();
	}
	while(isLetter(fileHndl->peek()) || isDigit(fileHndl->peek()) || fileHndl->peek() == '_');
	//empty the stack build a token and manage it
	extractToken(Token::IDENTIFIER);
      }
    
  };
  void read_next_char_to_stack(){
    char temp[1];
    fileHndl->read(temp, 1);
    charStack.push(*temp);
  };
  //need to pass the type of identifier to be passed to buildAndManageTokens
  void extractToken(int token_type){
    string tempStr = "";
    while(!charStack.empty())
      {
	tempStr.push_back(charStack.top());
	charStack.pop();
      }
    reverse(tempStr.begin(), tempStr.end());
    buildAndManageTokens(token_type, tempStr);
    throw TOKEN_FOUND;
    
  };
  void buildAndManageTokens(int token_type, string temp){
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
  void init_ops(){
    string op_str = "+,-,*,<,>,&,.,@,/,:,=,~,|,$,!,#,%,^,_,[,],{,},\",`,?";
    tokenize(op_str, ',', &operator_list);
  };
  
  void init_tokens(){
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
    try{
      handleEOF();
      handleIdentifier();
      handleIntegers();
      handleComments();
      handleOperators();
      handleStrings();
      handleSpaces();
      handlePunction();
    }
    catch(int ex_num){
      if(ex_num == TOKEN_FOUND){ 
	return; 
      }
      else if(ex_num == EOF_ENCOUNTERED){
	buildAndManageTokens(Token::ENDOFFILE, "EOF");
	return;
      }
      else{
	cout << "Lexer Error code :"  << ex_num << endl;
	throw ex_num;
      };
    }
    catch(...){ cout << "UnExpected Exception" << endl; exit(0);}
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
  Token *curr_tok, *prev_tok, *next_tok;
  vector<string> progContent;//fileContent
  ifstream *fileHndl;
  stack<char> charStack;//to store the input characters while making decisions
  vector<string> operator_list;
  char _ops[];
  static const int TOKEN_FOUND = 0;
  static const int EOF_ENCOUNTERED = 1;
};

