#include "helper.h"

#include "token.c++"
#include "lexer.c++"
#include "TreeNode.c++"
#include "control.c++"
#include "parser.c++"
#include "environment.c++"
#include "cse.c++"
#include "rpal.c++"


int main(int argc, char *argv[]){
  try{
  ifstream input_file;
  
  if(argc >= 3){
    string file_name = argv[2];
    input_file.open(argv[2]);
    if(NULL == input_file){
      cout << "File \"" << argv[2] << "\" not found!" << endl;
      return 0;
    };

    RpalLexer* lexer = new RpalLexer(&input_file);
    RpalParser* parser = new RpalParser(lexer);

    Rpal rpal(parser);

    if(strcmp(argv[1], "-ast") == 0){
      rpal.printAST();
    }
    else if(strcmp(argv[1], "-st") == 0){
      rpal.printST();
    }      
    else{
      rpal.execute();
    }
      input_file.close();
  }else{
    cout << "[RPAL Interpreter by Srinivas Muddana 06/02/2009]" << endl << endl << "Usage: rpal [-ast] [-st] [-r] filename" << endl << endl;
  };
  }
  catch(exception& ex){
    cout << ex.what() << endl;
  }
    return 0;
};
