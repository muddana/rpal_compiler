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

#include "lexer.h"

RpalLexer::RpalLexer(ifstream *fileHandle): fileHndl(fileHandle){
  setup();
};

const Token * RpalLexer::next_token(){
  return getScreenedNextToken();
};
