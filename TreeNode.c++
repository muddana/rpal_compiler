#include "TreeNode.h"

//pre-order treversal first root then the left and right child recursively
void TreeNode::pretty_print() const{
  _pretty_print(0);
};
string TreeNode::to_s() const{
  switch( _type )
    {
    case LET :
      return "let" ;
    case LAMBDA :
      return "lambda" ;
    case WHERE :
      return "where" ;
    case TAU :
      return "tau" ;
    case AUG :
      return "aug" ;
    case TERNARY :
      return "->" ;
    case OR :
      return "or" ;
    case AND_LOGICAL :
      return "&" ;
    case NOT :
      return "not" ;
    case GR :
      return "gr" ;
    case GE :
      return "ge" ;
    case LS :
      return "ls" ;
    case LE :
      return "le" ;
    case EQ :
      return "eq" ;
    case NE :
      return "ne" ;
    case ADD :
      return "+" ;		  
    case SUBTRACT :
      return "-" ;
    case NEG :
      return "neg" ;
    case MULTIPLY :
      return "*" ;		
    case DIVIDE :
      return "/" ;		
    case POWER :
      return "**" ;
    case AT :
      return "@" ;	  
    case GAMMA :
      return "gamma" ;
    case TRUE :
      return "<true>" ;
    case FALSE :
      return "<false>" ;	
    case NIL :
      return "<nil>" ;
    case DUMMY :
      return "<dummy>" ;
    case WITHIN :
      return "within" ;			
    case AND :
      return "and" ;
    case REC :
      return "rec" ;			
    case BINDING :
      return "=" ;
    case FCN_FORM :
      return "function_form" ;		
    case EMPTY_BRACKET :
      return "()" ;		
    case COMMA : 
      return "," ;
    case IDENTIFIER :					         
      if (_value == "nil") {						
	return "<nil>" ;}
      else
	return "<ID:" + _value + ">" ;
    case STRING :
      return "<STR:" + _value + ">" ;
    case INTEGER :
      return "<INT:" + _value + ">" ;
    case YSTAR:
      return "<Y*>";
    default :
      return "<UNKNOWN NODE>" ;
    }
};

void TreeNode::standardize(){
  _standardize();
};

void TreeNode::addChild(TreeNode *child){
  if(NULL == lft){
	lft = child;
  }
  else{
    TreeNode *temp = lft;
    while(NULL != temp->rgt){
      temp = temp->rgt;
    };
    temp->rgt = child;
  };
};
