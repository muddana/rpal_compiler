class Control{
public:
  void add_control(int , string,  vector<string> *, Control*, int); //cheating this int has to be TreeNode::Type
  void pretty_print();

  enum Type{
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
  };
  string to_s()
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
	return "G" ;
	
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

  Control( Control *cntrl )
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
  
  Control(Control::Type type, int index){
    if(type != Control::DELTA){
      cout << "Control::DELTA 's constructor called for : " << type << endl;
      throw "Control::DELTA 's constructor called for : ";
    };
    _type = type;
    _index = index;
    //initializing the control structure list
    _control_struct = new vector<Control *>();
  };
  
  Control(Control::Type type,vector<string> *variables, Control *del_ptr, int delta_index){
    _type = type;//Control::LAMBDA
    _index = delta_index;
    if(variables != NULL){
      for(int i=0;i<variables->size();i++){ 
	_variables.push_back(variables->at(i));
      };
    }
  };
  //For TAU and ENV
  Control(Control::Type type, int index, bool watever){
    if(type != Control::TAU && type != Control::ENV){
      cout << "Control::TAU 's constructor called for : " << type << endl;
      throw "Control::TAU 's constructor called for : ";
    };
    _type = type;
    _index = index;
    //initializing the control structure list
    //_control_struct = new vector<Control *>();
  };

  //make sure this is for NAME nodes only
  Control(string var_value, Control::Type type ){
    _type = type;
    _variables.push_back(var_value);
  };
  
  Control(Control::Type type, string value){
    _type = type;
    _value = value;
  };
  
  Control(){
  };

  //make sure this is used for AUG && NIL && YSTAR
  Control(Control::Type type){
    _type = type;
  };
  vector<Control *> *_control_struct;
  Type type(){
    return _type;
  };
  string value(){
    return _value;
  };
  int index(){
   return _index;
  }
  void set_linked_env_id(int id){
    _linked_env_id = id;
  };
  int linked_env_id(){
    return _linked_env_id;
  };
  void set_type(Type type){
    _type = type;
  };
  void set_value(string value){
    _value = value;
  }
  vector<string> _variables;
  vector<Control *> _tuples;
private:
  int _linked_env_id;
  Type _type;
  int _index;
  string _value;//need to verify the option to keep the _value as a string
  
  //following used only by deltas and lambdas for pointing to vector of control(structures) and the deltas respectively
  Control *_delta;
  
};
