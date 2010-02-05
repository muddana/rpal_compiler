//
// C++ Implementation: test
//
// Description: Parser which defines the grammer of rpal and enforces and implementes the tree AST building accordingly.
//
//
// Author: srinivas muddana, <smuddana@cise.ufl.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "lexer.c++"
#include "control.c++"
#include "naray_tree.c++"
#include "cse.c++"


class RpalParser{
 public:
  stack<TreeNode *> *parse(ifstream *input_file){
    lexer = new RpalLexer(input_file); 
    _parse();
    return &ast_stack;
  };

 private:

  RpalLexer *lexer;  
  Token *token;//curr_token
  Token *next_token;//for_peeking purposes
  stack<TreeNode *> ast_stack;


  void _parse(){
    //read the first token
    read_next_token();
    read_next_token();
    //every rpal prog is just an expression
    E();
    if(ast_stack.size() != 1){
      cout << "Oops more than 1 element on the stack" << endl;
      throw "Parse Error: Stack not empty at the end of parsing";
    }
    //also need to check if the input is also over
    if(next_token->type() != Token::ENDOFFILE){
      cout << "Input still remaining" << endl;
      throw "Parse Error: Input still remaining.";
    }
  };

  //always one token ahead feel like a good decision for a LL(1) parser
  void read_next_token(){
    token = next_token;
    next_token  = lexer->next_token();
  };

/*# Expressions ############################################
E    -> ’let’ D ’in’ E                          => ’let’
     -> ’fn’ Vb+ ’.’ E                          => ’lambda’
     ->  Ew;
*/
  void E(){ 
    if(token->value() == "let"){
      ReadToken("let");
      D();
      ReadToken("in");
      E();
      //build tree with let and two items on stack from D and E respectively
      build_tree(TreeNode::LET, 2);
    }
    else if(token->value() == "fn"){
      ReadToken("fn");
      int n = 0;
      do{
	Vb();
	n++;
      }while(token->value() == "(" || token->type() == Token::IDENTIFIER );
      ReadToken(".");
      E();
      build_tree(TreeNode::LAMBDA, n+1);
    }
    else{
      Ew();
    }
  };
  /*
Ew   -> T ’where’ Dr                            => ’where’
     -> T;*/
  void Ew(){
    T();
    if(token->value() == "where"){
      ReadToken("where");
      Dr();
      build_tree(TreeNode::WHERE, 2);
    }
  };
  /*
# Tuple Expressions ######################################
T    -> Ta ( ’,’ Ta )+                          => ’tau’
     -> Ta ;
  */
  void T(){
    Ta();
    if(token->value() == ","){
	int n = 0;
	do{
	  ReadToken(",");
	  Ta();
	  n++;
	}while(token->value() == ",");
	build_tree(TreeNode::TAU, n+1);
    }
      
  };

  /*
Ta   -> Ta ’aug’ Tc                             => ’aug’
     -> Tc ;
  */
  void Ta(){
    Tc();
    while(token->value() == "aug"){
      ReadToken("aug");
      Tc();
      build_tree(TreeNode::AUG, 2);
    }
  };

  /*
Tc   -> B ’->’ Tc ’|’ Tc                      => '->' 
     -> B ;
  */

  void Tc(){
    B();
    if(token->value() == "->"){
      ReadToken("->");
      Tc();
      ReadToken("|");
      Tc();
      build_tree(TreeNode::TERNARY, 3);
    }
  };
  /*
# Boolean Expressions ####################################
B    -> B ’or’ Bt                               => ’or’
     -> Bt ;
  */

  void B(){
    Bt();
    while(token->value() == "or"){
      ReadToken("or");
      Bt();
      build_tree(TreeNode::OR, 2);
    }
  };

  /*
Bt   -> Bt ’&’ Bs                               => ’&’
     -> Bs ;
  */
  void Bt(){
    Bs();
    while(token->value() == "&"){
      ReadToken("&");
      Bs();
      //build tree with 2 elements from the stack
      build_tree(TreeNode::AND_LOGICAL, 2);
    }
  };
  
  /*
Bs   -> ’not’ Bp                                => ’not’
     -> Bp ;
  */
  void Bs(){
    if(token->value() == "not"){
      ReadToken("not");
      Bp();
      build_tree(TreeNode::NOT, 1);
    }
    else{
      Bp();
    }
  };
  
  /*
Bp   -> A (’gr’ | ’>’ ) A                       => ’gr’
     -> A (’ge’ | ’>=’) A                       => ’ge’
     -> A (’ls’ | ’<’ ) A                       => ’ls’
     -> A (’le’ | ’<=’) A                       => ’le’
     -> A ’eq’ A                                => ’eq’
     -> A ’ne’ A                                => ’ne’
     -> A ;
  */
  void Bp(){
    A();
    if(token->value() == "gr" || token->value() == ">" ){
      ReadToken(token->value());
      A();
      build_tree(TreeNode::GR, 2);
    }
    else if(token->value() == "ge" || token->value() == ">="){
      ReadToken(token->value());
      A();
      build_tree(TreeNode::GE, 2);
    }
    else if(token->value() == "ls" || token->value() == "<"){
      ReadToken(token->value());
      A();
      build_tree(TreeNode::LS, 2);
    }
    else if(token->value() == "le" || token->value() == "<="){
      ReadToken(token->value());
      A();
      build_tree(TreeNode::LE, 2);
    }
    else if(token->value() == "eq"){
      ReadToken(token->value());
      A();
      build_tree(TreeNode::EQ, 2);
    }
    else if(token->value() == "ne"){
      ReadToken(token->value());
      A();
      build_tree(TreeNode::NE, 2);
    }
  };

 /*
# Arithmetic Expressions #################################
A    -> A ’+’ At                                => ’+’
     -> A ’-’ At                                => ’-’
     ->   ’+’ At
     ->   ’-’ At                                => ’neg’
     -> At ;
  */
  void A(){
    if(token->value() == "+"){
      ReadToken("+");
      At();
    }
    else if(token->value() == "-"){
      ReadToken("-");
      At();
      build_tree(TreeNode::NEG, 1);
    }
    else{
      At();
    }
    while(token->value() =="+" || token->value()=="-"){
      string temp_tok_value = token->value();
      ReadToken(token->value());//either +  or -
      At();
      if(temp_tok_value != "+" &&  temp_tok_value != "-"){
	cout << "Expecting + or - but recieved: " <<  temp_tok_value << endl;
	throw "Expecting + or - but recieved: " +  temp_tok_value;
      };
      build_tree(temp_tok_value == "+" ? TreeNode::PLUS : TreeNode::MINUS , 2);
    };
  };
  /*
At   -> At ’*’ Af                               => ’*’
     -> At ’/’ Af                               => ’/’
     -> Af ;
  */
  void At(){
    Af();
    while(token->value() == "*" || token->value() == "/"){
      string temp_tok_value = token->value();
      ReadToken(token->value());
      Af();
      build_tree(temp_tok_value == "*" ? TreeNode::MULTIPLY : TreeNode::DIVIDE , 2);
    }
  };

  /*
Af   -> Ap ’**’ Af                              => ’**’
     -> Ap ;
  */
  void Af(){
    Ap();
    if(token->value() == "**"){
      ReadToken("**");
      Af();
      build_tree(TreeNode::POWER , 2);
    }
  };

/*
Ap   -> Ap ’@’ ’<IDENTIFIER>’ R                 => ’@’
     -> R ;
  */
  void Ap(){
    R();
    while(token->value() == "@"){
      ReadToken("@");
      build_tree(TreeNode::IDENTIFIER, token->value());
      ReadToken(token->value());
      R();
      build_tree(TreeNode::AT , 3);
    }
  };

/*# Rators And Rands #######################################
R    -> R Rn                                    => ’gamma’
     -> Rn ;
*/
  void R(){
    Rn();
    while((token->type() == Token::IDENTIFIER || token->type() == Token::INTEGER || token->type() == Token::STRING || token->value() == "(" || token->value() == "false" || token->value() == "true" || token->value() == "nil" || token->value() == "dummy") && !is_keyword(token->value())){
      Rn();
      build_tree(TreeNode::GAMMA , 2);
    };
  };

/*
Rn   -> ’<IDENTIFIER>’
     -> ’<INTEGER>’
     -> ’<STRING>’
     -> ’true’                                  => ’true’
     -> ’false’                                 => ’false’
     -> ’nil’                                   => ’nil’
     -> ’(’ E ’)’
     -> ’dummy’                                 => ’dummy’ ; 
*/
  void Rn(){
    if(token->value() == "(" && token->type() == Token::PUNCTION){
      ReadToken("(");
      E();
      ReadToken(")");
    }
    else if(token->type() == Token::IDENTIFIER || token->type() == Token::INTEGER || token->type() == Token::STRING){
      if(token->value() == "true"){
	build_tree(TreeNode::TRUE, token->value());
	ReadToken("true");
      }
      else if(token->value() == "false"){
	build_tree(TreeNode::FALSE, token->value());
	ReadToken("false");
      }
      else if(token->value() == "nil"){
	build_tree(TreeNode::NIL, token->value());
	ReadToken("nil");
      }
      else if(token->value() == "dummy"){
	build_tree(TreeNode::DUMMY, token->value());
	ReadToken("dummy");
      }
      else if(token->type() == Token::IDENTIFIER){
	build_tree(TreeNode::IDENTIFIER, token->value());
	ReadToken(token->value());
      }
      else if(token->type() == Token::STRING){
	build_tree(TreeNode::STRING, token->value());
	ReadToken(token->value());
      }
      else if(token->type() == Token::INTEGER){
	build_tree(TreeNode::INTEGER, token->value());
	ReadToken(token->value());
      }
      else
	throw("Token Not Recognized: " + token->value());
    }
    else{
      throw("Token Not Recognized: " + token->value());
    };
  };

  /*
# Definitions ############################################
D    -> Da ’within’ D                           => ’within’
     -> Da ;
  */
  void D(){
    Da();
    if(token->value() == "within"){
      ReadToken("within");
      D();
      build_tree(TreeNode::WITHIN, 2);
    }
  };

  /*
    Da   -> Dr ( ’and’ Dr )+                        => ’and’
         -> Dr ;
  */
  void Da(){
    Dr();
    if(token->value() == "and"){
      int n = 0;
      do{
	ReadToken("and");
	n++;
	Dr();
      }while(token->value() == "and");
      build_tree(TreeNode::AND, n+1);
    }
  };


  /*
    Dr   -> ’rec’ Db                                => ’rec’
         -> Db ;
  */
  void Dr(){
    if(token->value() == "rec"){
      ReadToken("rec");
      Db();
      build_tree(TreeNode::REC, 1);
    }
    else{
      Db();
    }
     
  }

  /*
    Db   -> Vl ’=’ E                              => ’=’
         -> ’<IDENTIFIER>’ Vb+ ’=’ E              => ’fcn_form’
         -> ’(’ D ’)’ ;
  */
  void Db(){
    if(token->value() == "("){
      ReadToken("(");
      D();
      ReadToken(")");
    }
    else if(token->type() == Token::IDENTIFIER){
      //peeking ahead
      if(next_token->value() == "=" || next_token->value() == ","){
	Vl();
	ReadToken("=");
	E();
	build_tree(TreeNode::BINDING, 2);
      }
      else{
	build_tree(TreeNode::IDENTIFIER, token->value());
	ReadToken(token->value());
	int n = 0;
	do{
	  Vb();
	  n++;
	}while(token->value() == "(" || token->type() == Token::IDENTIFIER);
	ReadToken("=");
	E();
	build_tree(TreeNode::FCN_FORM, n+2);
      }
    }
    else{
      throw "Expected IDENTIFIER in Db";
    }
  };

  /*
    # Variables ##############################################
    Vb   -> ’<IDENTIFIER>’
         -> ’(’ Vl ’)’
         -> ’(’ ’)’                                 => ’()’;
  */
  void Vb(){
    if(token->value() == "("){
      ReadToken("(");
      if(token->value() == ")"){
	ReadToken(")");
	build_tree(TreeNode::EMPTY_BRACKET, 0);
      }
      else{
	Vl();
	ReadToken(")");
      }
	
    }
    else if(token->type() == Token::IDENTIFIER){
      build_tree(TreeNode::IDENTIFIER, token->value());
      ReadToken(token->value());
    }
    else{
      throw "Expected '(' or IDNETIFIER but got" + token->value();
    }  
  };

  /*
    Vl   -> ’<IDENTIFIER>’ list ’,’                 => ’,’?;
  */
  void Vl(){
    build_tree(TreeNode::IDENTIFIER, token->value());
    ReadToken(token->value());
    if(token->value() == ","){
      int n = 0;
      do{
	ReadToken(",");
	build_tree(TreeNode::IDENTIFIER, token->value());
	ReadToken(token->value());
	n++;
      }while(token->value() == ",");
      build_tree(TreeNode::COMMA, n+1);
    }
  };

  //val is the expected value of the token(current token)
  void ReadToken(string val){
    if(token->value() == val){
      read_next_token();
    }
    else{
      cout << "Expected '" + val + "' recieved '" + token->value() << "' token type : " << token->type() << endl;
      throw "Expected " + val + " recieved " + token->value(); 
    }
  };

  //num_stack_ele is the number of stack elements to pop n build a node n then push on to the stack
  void build_tree(TreeNode::Type type, int num_stack_ele){
    TreeNode *new_node = new TreeNode(type);
    if(num_stack_ele > 0){
      TreeNode *temp_lft_child = ast_stack.top();
      ast_stack.pop();
      for(int i= 1; i< num_stack_ele; i++){
	ast_stack.top()->rgt = temp_lft_child;
	temp_lft_child = ast_stack.top(); 
	ast_stack.pop();
      }
      new_node->lft = temp_lft_child;
    }
    ast_stack.push(new_node);
  };

  //to build basic nodes like INTEGER, IDENTIFIER and STRING
  void build_tree(TreeNode::Type type, string ele_val){
    ast_stack.push(new TreeNode(ele_val, type));
  };

  bool is_keyword(string val){
    string _key_words[] = { "in", "where", ".", "aug", "and", "or", "&", "not", "gr", "ge", "ls", "le", "eq", "ne", "+", "-",     "*", "/", "**", "@", "within", "=", "rec", "let", "fn"}; //25 elements
    vector<string> key_words (_key_words, _key_words + sizeof(_key_words) / sizeof(string) );
    if(find(key_words.begin(), key_words.end(), val) != key_words.end())
      return true;
    else
      return false;
  };
};

int main(int argc, char *argv[]){
  ifstream input_file;
  stack<TreeNode *> *ast_stack;
  RpalParser *parser = new RpalParser();
  CSE *cse_machine = new CSE();

  if(argc >= 3){
    string file_name = argv[2];
    input_file.open(argv[2]);
    if(input_file == NULL){
      cout << "File \"" << argv[2] << "\" not found!" << endl;
      return 0;
    };
    if(strcmp(argv[1], "-ast") == 0){
      ast_stack = parser->parse(&input_file);
      ast_stack->top()->pretty_print();
    }
    else if(strcmp(argv[1], "-st") == 0){
      ast_stack = parser->parse(&input_file);
      ast_stack->top()->standardize();
      ast_stack->top()->pretty_print();
    }      
    else{
      ast_stack = parser->parse(&input_file);
      ast_stack->top()->standardize();
      cse_machine->run(ast_stack->top());
    }
    if(input_file != NULL){
      input_file.close();
    };
  }else{
    cout << "[RPAL Interpreter by Srinivas Muddana 06/02/2009]" << endl << endl << "Usage: rpal [-ast] [-st] [-r] filename" << endl << endl;
  };
    return 0;
};
  
