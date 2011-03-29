#include "helper.h"

#include "token.c++"
#include "lexer.c++"
#include "control.c++"
#include "TreeNode.c++"
#include "parser.c++"
#include "cse.c++"

int main(int argc, char *argv[]){
  try{
  ifstream input_file;
  stack<TreeNode *> *ast_stack;
  // = NULL; 
  RpalParser *parser = new RpalParser();
  CSE *cse_machine = new CSE();

  if(argc >= 3){
    string file_name = argv[2];
    input_file.open(argv[2]);
    if(NULL == input_file){
      cout << "File \"" << argv[2] << "\" not found!" << endl;
      return 0;
    };

    RpalLexer * const lexer = new RpalLexer(&input_file);
    if(strcmp(argv[1], "-ast") == 0){
      ast_stack = parser->parse(lexer);
      ast_stack->top()->pretty_print();
    }
    else if(strcmp(argv[1], "-st") == 0){
      ast_stack = parser->parse(lexer);
      ast_stack->top()->standardize();
      ast_stack->top()->pretty_print();
    }      
    else{
      ast_stack = parser->parse(lexer);
      ast_stack->top()->standardize();
      cse_machine->run(ast_stack->top());
    }
      input_file.close();
  }else{
    cout << "[RPAL Interpreter by Srinivas Muddana 06/02/2009]" << endl << endl << "Usage: rpal [-ast] [-st] [-r] filename" << endl << endl;
  };
  }
  catch(exception& ex){
    //cout << ex.what() << endl;
  }
    return 0;
};
