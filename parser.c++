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

#include "parser.h"

RpalParser::RpalParser(RpalLexer* lexer): _lexer(lexer){
};

stack<TreeNode *> *RpalParser::parse(){
  _parse();
  return &ast_stack;
};
