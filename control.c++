#include "control.h"

#include "TreeNode.h"

/*enum Control::Type{
    ENV = 1 ,
    DELTA = 2 ,
    NAME = 3 ,
    LAMBDA = 4 ,
    GAMMA = 5 ,
    AUG = 6 ,
    BETA = 7 ,
    OR = 8 ,
    AND_LOGICAL = 9 ,
    NOT = 10 ,
    GR = 11 ,
    GE = 12 ,
    LS = 13 ,
    LE = 14 ,
    EQ = 15 ,
    NE = 16 ,
    ADD = 17 ,
    SUBTRACT = 18 ,
    NEG = 19 ,
    MULTIPLY = 20 ,
    DIVIDE = 21 ,
    EXP = 22 ,
    AT = 23 ,
    TRUE = 24 ,
    FALSE = 25 ,
    NIL = 26 ,
    DUMMY = 27 ,
    YSTAR = 28 ,
    ETA = 29 ,
    TAU = 30 ,
    STRING = 31 ,
    INTEGER = 32 ,
    TUPLE = 33
    };*/
  string Control::to_s()
  {
    //cout << "Type: " << _type << endl; 
    string str ;
    switch( _type )
      {
      case DELTA :
	str = "<D" + itos(_index) + ">" ;
	return str;
	
      case NAME :
	return _variables.at(0);//.front() ; //not sure y front() is not working
	
      case LAMBDA :
	str = "[lambda closure: " ;
	for( int i = 0 ; i < _variables.size() ; i++ )
	  str += _variables.at(i) + ": " ;
	str += itos(_index) + "]" ;
	return str ;
	
      case GAMMA :
	return "Gamma" ;
	
      case AUG :
	return "AUG" ;
	
      case BETA :
	return "BETA" ;
	
      case OR :
	return "OR" ;
	
      case AND_LOGICAL :
	return "AND" ;
	
      case NOT :
	return "NOT" ;
	
      case GR :
	return ">" ;
	
      case GE :
	return ">=" ;
	
      case LS :
	return "<" ;
	
      case LE :
	return "<=" ;
	
      case EQ :
	return "=" ;
	
      case NE :
	return "!=" ;
	
      case ADD :
	return "+" ;
	
      case SUBTRACT :
	return "-" ;
	
      case NEG :
	return "NEG" ;
	
      case MULTIPLY :
	return "*" ;
	
      case DIVIDE :
	return "/" ;
	
      case EXP :
	return "**" ;
	
      case AT :
	return "@" ;
	
      case TRUE :
	return "true" ;
	
      case FALSE :
	return "false" ;
	
      case NIL :
	return "nil" ;
	
      case DUMMY :
	return "dummy" ;
	
      case YSTAR :
	return "Y" ;
	
      case TAU :
	str = "<T" + itos(_index) + ">"  ;
	return str;
	
      case STRING :
	return _value +"";
	
      case INTEGER :
	str = _value + "" ;
	return str;
	
      case ENV :
	str = "e" + itos(_index) ;
	return str;
	
      case ETA :
	str = "<ETA," + itos(_index) ;
	for( int i = 0 ; i < _variables.size() ; i++ )
	  str += "," + _variables.at(i) ;
	str += ">" ;
	return str ;
	
      case TUPLE :
	str = "(" ;
	for( int i = 0 ; i < _tuples.size() ; i++ )
	  {
	    str += _tuples.at(i)->to_s() ;
	    if( i != _tuples.size()-1 )
	      str += ", " ;
	  }
	str += ")" ;
	return str ;
	
      default :
	return "<UNKNOWN CONTROL>" ;
      }
  };

  Control::Control( Control *cntrl )
  {
    _index = cntrl->index() ;
    _value = cntrl->value() ;
    _type = cntrl->type() ;
    _linked_env_id = cntrl->linked_env_id();
    if( !cntrl->_variables.empty() ){
      for(int i=0;i<cntrl->_variables.size();i++)
	_variables.push_back(cntrl->_variables.at(i)) ;
    }
    if( !cntrl->_tuples.empty() ){
      for(int i=0;i<cntrl->_tuples.size();i++)
	_tuples.push_back(cntrl->_tuples.at(i)) ;
    }
  };
  
  Control::Control(Control::Type type, int index){
    if(Control::DELTA != type){
      throw RpalError(RpalError::INTERNAL,"Control::DELTA 's constructor called for : " + type);
    };
    _type = type;
    _index = index;
    //initializing the control structure list
    _control_struct = new vector<Control *>();
  };
  
  Control::Control(Control::Type type,vector<string> *variables, Control *del_ptr, int delta_index){
    _type = type;//Control::LAMBDA
    _index = delta_index;
    if(variables != NULL){
      for(int i=0;i<variables->size();i++){ 
	_variables.push_back(variables->at(i));
      };
    }
  };
  //For TAU and ENV
  Control::Control(Control::Type type, int index, bool watever){
    if(type != Control::TAU && type != Control::ENV){
      throw RpalError(RpalError::INTERNAL, "Control::TAU 's constructor called for : "  + type );
    };
    _type = type;
    _index = index;
    //initializing the control structure list
    //_control_struct = new vector<Control *>();
  };

  //make sure this is for NAME nodes only
  Control::Control(string var_value, Control::Type type ){
    _type = type;
    _variables.push_back(var_value);
  };
  
  Control::Control(Control::Type type, string value){
    _type = type;
    _value = value;
  };
  
  Control::Control(){
  };

  //make sure this is used for AUG && NIL && YSTAR
  Control::Control(Control::Type type){
    _type = type;
  };

Control::Type Control::type(){
    return _type;
  };
  string Control::value(){
    return _value;
  };
  int Control::index(){
   return _index;
  }
  void Control::set_linked_env_id(int id){
    _linked_env_id = id;
  };
  int Control::linked_env_id(){
    return _linked_env_id;
  };
void Control::set_type(Type type){
    _type = type;
  };
void Control::set_value(string value){
    _value = value;
  }



//should this be avaiblable for the delta node ?
void Control::pretty_print(){
  if(_type!=Control::DELTA){
    throw RpalError(RpalError::INTERNAL,"pretty_print called on non delta node, Not a delta node, cannot pretty print");
  };
  cout << to_s() << " ";
  for(int i=0; i< _control_struct->size(); i++){
    cout << _control_struct->at(i)->to_s() << " " ;
  };
  cout << endl;
};


//TODO: why not pass the whole treenode itself?
void Control::add_control(int type, string value, vector<string> *variables, Control* del_ptr, int deltas_size){ 
    //has to store appropriate control depeneding on the type and value of the tree node

  //cout << "TreeNode " << node->to_s() << " : "  << node->value()<< endl;

  int tau_count;
  Control *temp = NULL;
  switch(type){
  case TreeNode::LAMBDA:
    temp = new Control(Control::LAMBDA, variables, del_ptr, deltas_size-1 );
    break;
  case TreeNode::INTEGER:
    temp = new Control(Control::INTEGER, value);
    break;
  case TreeNode::MULTIPLY:
    temp = new Control(Control::MULTIPLY, value);
    break;
  case TreeNode::ADD:
    temp = new Control(Control::ADD, value);
    break;
  case TreeNode::SUBTRACT:
    temp = new Control(Control::SUBTRACT, value);
    break;
  case TreeNode::DIVIDE:
    temp = new Control(Control::DIVIDE, value);
    break;
  case TreeNode::GAMMA:
    temp = new Control(Control::GAMMA, value);
    break;
  case TreeNode::IDENTIFIER:
    //cout << "adding new identifier control " << value<< endl;
    temp = new Control(value, Control::NAME);
    break;
  case TreeNode::STRING:
    //cout << " String control of Value is : "<< value << endl;
    temp = new Control(Control::STRING, value.substr(1,value.length()-2));
    break;
  case TreeNode::TAU:
    if(variables!=NULL)
      tau_count = variables->size();
    else
      cout << "TAU add_control NULL variables sent!" << endl;
    temp = new Control(Control::TAU, tau_count, false);
    break;
  case TreeNode::AUG:
    temp = new Control(Control::AUG);
    break;
  case TreeNode::NIL:
    temp = new Control(Control::NIL);
    break;
  case TreeNode::YSTAR:
    temp = new Control(Control::YSTAR);
    break;
  case TreeNode::AND_LOGICAL:
    temp = new Control(Control::AND_LOGICAL);
    break;
  case TreeNode::OR:
    temp = new Control(Control::OR);
    break;
  case TreeNode::NE:
    temp = new Control(Control::NE);
    break;
  case TreeNode::EQ:
    temp = new Control(Control::EQ);
    break;
  case TreeNode::LS:
    temp = new Control(Control::LS);
    break;
  case TreeNode::LE:
    temp = new Control(Control::LE);
    break;
  case TreeNode::GR:
    temp = new Control(Control::GR);
    break;
  case TreeNode::GE:
    temp = new Control(Control::GE);
    break;
  case TreeNode::NEG:
    temp = new Control(Control::NEG);
    break;
  case TreeNode::FALSE:
    temp = new Control(Control::FALSE);
    break;
  case TreeNode::TRUE:
    temp = new Control(Control::TRUE);
    break;
  case TreeNode::NOT:
    temp = new Control(Control::NOT);
    break;
  case TreeNode::DUMMY:
    temp = new Control(Control::DUMMY);
    break;
  default:
    throw RpalError(RpalError::INTERNAL,"UnHandled TreeNode found!?, TreeNode value:" + value + " type:" + itos(type));
    break;
  };
  _control_struct->push_back(temp);
  //cout << "added control " << temp->to_s() << " : "  << temp->value()<< endl;
};


