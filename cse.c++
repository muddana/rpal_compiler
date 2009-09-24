#include<map>

class Environment{
public:
  void assign_parent(Environment *parent){
    _parent = parent;
  };

  Control* lookup(string symbol){
    //cout << "looking up for : " << symbol << " in : " << id() << endl;
    //pretty_print();
    map<string, Control *>::iterator it;
    it = symbol_table.find(symbol);
    //cout << "it is: " << (*it).second->to_s() << endl;
    if(it == symbol_table.end())
       return _parent->lookup(symbol);
    else
      return (*it).second;
  };
  Environment(int id){
    _id = id;
  };
  int id(){
    return _id;
  };
  void pretty_print(){
    map<string, Control *>::iterator it;
    for ( it=symbol_table.begin() ; it != symbol_table.end(); it++ )
      cout << (*it).first << " => " << (*it).second->to_s() << endl;
  };
  map<string, Control *> symbol_table;
private:
  int _id;
  Environment *_parent;
  //should have all the primitive env data like common functions like -, + etc.
};

//control(stack) stack environment
class CSE{
public:
  CSE(){
    env_count = -1; // count starts from -1 which will correspond in the id of the environment
    //therefore PE will have id -1 and rest in increments of 1
    primitive_env = create_new_env();
    env_stack.push(primitive_env);
    deltas = new vector<Control *>;
    root_delta = new Control(Control::DELTA, deltas->size());
  };

  void run(TreeNode *root){
    init_control_stack(root);
    _run();
    //cout << "Final CSE state: " << endl;
    //print_control_and_stack();
    if(!_control.empty() || env_stack.size() != 1){
      cout << "Interpretor Error: stack is not empty of the env_stack is not empty" << endl;
    }
    cout << endl;
  };
private:
  //temp_env used only by create_new_env
  Environment *temp_env;
  Environment *create_new_env(){
    temp_env = new Environment(env_count);
    //cout << "env_count is: " << env_count << endl;
    //cout << "ENV id is : " << temp_env->id() << " and env_count is: " << env_count  <<endl;
    env_hash[env_count] =  temp_env;
    env_count++;
    return temp_env;
  };

  void print_control_and_stack(){
    for(int i=0; i < _control.size() ; i++){
      cout << _control.at(i)->to_s() << " " ;
    };
    cout << "|||||";
    stack<Control *> temp;
    while(!_stack.empty()){
      cout << _stack.top()->to_s() << " " ;
      temp.push(_stack.top());
      _stack.pop();
    };
    cout << endl;
    //refilling the stack
    while(!temp.empty()){
      _stack.push(temp.top());
      temp.pop();
    };
    //while(!env_stack.empty()){
      
    //};
  };
  //preparing the control structures and putting the first control structure on the control
  void init_control_stack(TreeNode *root){
    deltas->push_back(root_delta);
    root->flatten(root_delta, deltas);
    //cout << "about to print the control structures" << endl;
    //for(int i=0; i< deltas->size(); i++) 
    //  deltas->at(i)->pretty_print();
    //begin processing the control structures
    //push the env control on the _control and _stack
    _control.push_back(new Control(Control::ENV, 0, false));
    _stack.push(new Control(Control::ENV, 0, false));
    for(int i=0;i<root_delta->_control_struct->size();i++){
      _control.push_back(root_delta->_control_struct->at(i));
    };

    //build a new environment for the control structure put on control
    env_stack.push(create_new_env());
    env_stack.top()->assign_parent(primitive_env);
    //setting the variable curr_env
    curr_env = env_stack.top();
  };
  void _run(){
    Control *temp = NULL;
    Control *curr_control = NULL;
    Control *rator = NULL;
    Environment *new_env = NULL;
    int delta_index = -1;
    while(!_control.empty()){
      //print_control_and_stack();
      curr_control = _control.at(_control.size()-1); 
      temp = NULL;
      //cout << "Type: " << curr_control->type() << endl;
      switch(curr_control->type()){
      case Control::INTEGER :
      case Control::STRING :
      case Control::TRUE :
      case Control::FALSE :
      case Control::DUMMY :
      case Control::YSTAR :
	put_on_stack_pop_from_control(curr_control);
	break ;
      case Control::NAME:
	if(is_inbuilt_function(curr_control->_variables.front())){
	  //cout << "Inbuilt function found : " << curr_control->value() << endl;
	  put_on_stack_pop_from_control(curr_control);
	}
	else{
	  //cout << "Looking up for : " << curr_control->_variables.front() << endl;
	  temp = curr_env->lookup(curr_control->_variables.front());
	  //cout << "Search ended " << endl;
	  if(temp != NULL){
	    //cout << "Found : " << endl;
	    //cout <<  temp << endl;
	    put_on_stack_pop_from_control(temp);
	  }
	  else{
	    cout << "Unknown name: " << curr_control->value() << endl;
	    throw "Unknown name" ;
	  };
	};
	break;
      case Control::NIL :
	curr_control->_tuples.clear();
	put_on_stack_pop_from_control(curr_control);
	break ;
      case Control::LAMBDA : //CSE Rule 2
	curr_control->set_linked_env_id(curr_env->id());
	put_on_stack_pop_from_control(curr_control);
	break ;
      case Control::GAMMA:
	_control.pop_back();	
	rator = _stack.top() ;
	_stack.pop();
	if( rator->type() == Control::LAMBDA ){ //CSE Rule 4 and Rule 11
	  //creating a new env
	  new_env = create_new_env();
	  //assigning a new parent under which it was defined
	  new_env->assign_parent(env_hash.find(rator->linked_env_id())->second);
	  //setting new env as curr env
	  curr_env = new_env ;
	  
	  if( rator->_variables.size() == 1 ){
	    //cout << "In ENV: " << curr_env->id() << " Binding : " << rator->_variables.at(0) << " with _stack.top() value: " << _stack.top()->to_s() << endl ;
	    curr_env->symbol_table[rator->_variables.at(0)] = _stack.top();
	    _stack.pop();
	  }
	  else{
	    temp = _stack.top(); //should contain the tuple
	    _stack.pop() ; 
	    if( temp->type() == Control::TUPLE && rator->_variables.size() == temp->_tuples.size() )
	      {
		for( int i = 0 ; i < rator->_variables.size() ; i++ )
		  {
		    curr_env->symbol_table[rator->_variables.at(i)] = temp->_tuples.at(i);
		  }
	      }
	    else
	      {
		cout << "Number/Type of arguments to a function doesn't match. rator variable size: "  << rator->_variables.size() << " temp tuples->size:" << temp->_tuples.size() << endl;
		exit(1) ;
	      }
	  };
	  
	  env_stack.push(curr_env);
	  //begin processing the control structures
	  //push the env control on the _control and _stack
	   _control.push_back(new Control(Control::ENV, curr_env->id(), false));
	   _stack.push(new Control(Control::ENV, curr_env->id(), false));
	   for(int i=0;i<deltas->at(rator->index())->_control_struct->size();i++){
	     _control.push_back(deltas->at(rator->index())->_control_struct->at(i));
	   }
	}
	else if( rator->type() == Control::YSTAR ){ //Rule 12
	  Control *eta = new Control( _stack.top() ) ;
	  _stack.pop() ;
	  eta->set_type(Control::ETA) ;
	  _stack.push(eta) ; 
	}
	else if( rator->type() == Control::ETA ){ //Rule 13
	  _control.push_back( new Control(Control::GAMMA) ) ;
	  _control.push_back( new Control(Control::GAMMA) ) ;
	  _stack.push(rator) ;
	  Control *lambda = new Control(Control::LAMBDA, &(rator->_variables), NULL, rator->index()) ;
	  lambda->set_linked_env_id(rator->linked_env_id()) ;
	  _stack.push(lambda) ;
	}
	else if( rator->type() == Control::TUPLE){ //Rule 10
	  if( _stack.top()->type() == Control::INTEGER ){
	    temp = rator->_tuples.at(atoi(_stack.top()->value().c_str()) - 1) ;
	    _stack.pop();
	    _stack.push(temp) ;
	  }
	  else{
	    cout << "Expected an integer while indexing tuples!";
	    exit(1) ;
	  }
	}
	else{
	  //cout << "Going for apply rator, type:  " << rator->type() << " value:%" << rator->to_s() << "%" << endl;
	  apply_rator(rator);
	};
	break;
      case Control::NOT :
	_control.pop_back();
	temp = _stack.top() ;
	if( temp->type() == Control::TRUE || temp->type() == Control::FALSE ){
	  temp->set_type( temp->type() ==  Control::TRUE ? Control::FALSE : Control::TRUE) ;
	}
	else{
	  cout << "Expecting a bool value for operator 'not'" << endl;
	  exit(1) ;
	}				
	break ;
	//CSE Rule 6
      case Control::GR :
      case Control::GE :
      case Control::LS :
      case Control::LE :
      case Control::EQ :
      case Control::NE : 
      case Control::AUG :
      case Control::OR :
      case Control::AND_LOGICAL : 
      case Control::ADD :
      case Control::SUBTRACT :
      case Control::MULTIPLY :
      case Control::DIVIDE :
      case Control::EXP :
	apply_binary_operation(curr_control->type()) ;
	break ;
      case Control::TAU:
	_control.pop_back();
	temp = new Control() ;
	temp->set_type(Control::TUPLE) ;
	temp->_tuples.clear();
	for(int i=0; i<curr_control->index(); i++ )
	  {
	    temp->_tuples.push_back(_stack.top()) ;
	    _stack.pop() ;
	  }
	_stack.push(temp) ;
	break ;
      case Control::NEG : //CSE Rule 7
	_control.pop_back();
	if( _stack.top()->type() == Control::INTEGER )
	  _stack.top()->set_value(itos(-1*atoi(_stack.top()->value().c_str()))) ;
	else
	  {
	    cout << "Neg: Expected Interger!" << endl;
	    exit(1) ;
	  }
	break ;
      case Control::ENV:
	temp = _stack.top() ;
	_stack.pop();
	if( _stack.top()->type() == Control::ENV && _stack.top()->index() == curr_control->index() ){
	    _control.pop_back();
	    _stack.pop();
	    _stack.push(temp) ;

	    env_stack.pop() ;
	    curr_env = env_stack.top() ;
	  }
	else{
	  cout << "Environment markers do not match!" << endl;
	    exit(1) ;
	}
	break;
      case Control::BETA:
	_control.pop_back(); //popping beta
	if(_stack.top()->type() == Control::TRUE){
	  _control.pop_back(); //popping else's delta
	  if(_control.at(_control.size() - 1)->type() == Control::DELTA){
	    delta_index = _control.at(_control.size() -1)->index();
	  _control.pop_back(); //popping then's delta
	  }
	  else{
	    cout << "Delta Expected " << _control.at(_control.size() -1)->type() << endl;
	    exit(1);
	  } 
	}
	else if(_stack.top()->type() == Control::FALSE){
	  if(_control.at(_control.size() - 1)->type() == Control::DELTA){
	    delta_index = _control.at(_control.size() -1)->index();
	    _control.pop_back(); //popping else's delta
	    _control.pop_back(); //popping then's delta
	  }
	  else{
	    cout << "Delta Expected, found, " << _control.at(_control.size() -1)->type() << endl;
	    exit(1);
	  }
	}
	else{
	  cout << " '->' operator Expected a bool value"  << endl;
	  exit(1);
	}
	//popping the stack
	_stack.pop();
	for(int i=0; i< deltas->at(delta_index)->_control_struct->size() ; i++){
	  _control.push_back(deltas->at(delta_index)->_control_struct->at(i));
	  };
	break;
      default:
	cout << "Interpretor Error: Unknown Control Type: " << curr_control->type() << endl;
	throw "Interpretor Error: Unknown Control Type";
	break;
      };
    }
  };
  
  void apply_binary_operation( int type )
  {
    _control.pop_back();
    Control *rand1 = _stack.top() ;
    _stack.pop();
    Control *rand2 = _stack.top() ;
    _stack.pop();
    Control *result = new Control() ;
    result->set_type(Control::INTEGER);
    switch( type )
      {

      case Control::AUG :
	result->set_type(Control::TUPLE) ;
	result->_tuples;
	if( rand1->type() != Control::NIL ){
	  if( rand1->type() == Control::TUPLE){
	    for(int i=0;i<rand1->_tuples.size(); i++)
	      result->_tuples.push_back(rand1->_tuples.at(i));
	  }
	  else{
	    result->_tuples.push_back(rand1) ;
	  }
	}
	if( rand2->type() != Control::NIL ){
	  if( rand2->type() == Control::TUPLE){
	    for(int i=0;i<rand2->_tuples.size(); i++)
	      result->_tuples.push_back(rand2->_tuples.at(i));
	  }
	  else
	    result->_tuples.push_back(rand2);
	}
	break ;
      case Control::EXP :
	if(rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER) {
	    result->set_value(itos(pow(atoi(rand1->value().c_str()), atoi(rand2->value().c_str())))) ;
	    break ;
	  }
	else {
	    cout << "Incompatible arguments for the operator '**'" ;
	    exit(1) ;
	  }
	break ;
      case Control::GR :
	if(rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER){
	  if( atoi(rand1->value().c_str()) > atoi(rand2->value().c_str()) )
	    result->set_type(Control::TRUE) ;
	  else
	    result->set_type(Control::FALSE) ;
	  break ;
	}
	else{
	  cout << "Incompatible arguments for the operator 'GR'" << endl ;
	  exit(1) ;
	}
	break ;
      case Control::GE :
	if(rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER) {
	    if( atoi(rand1->value().c_str()) >= atoi(rand2->value().c_str()) )
	      result->set_type(Control::TRUE) ;
	    else
	      result->set_type(Control::FALSE) ;
	    break ;
	  }
	else {
	    cout << "Incompatible arguments for the operator 'GE'" ;
	    exit(1) ;
	  }
	break ;
      case Control::LS :
	if(rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER) {
	    if( atoi(rand1->value().c_str()) < atoi(rand2->value().c_str()) )
	      result->set_type(Control::TRUE) ;
	    else
	      result->set_type(Control::FALSE) ;
	    break ;
	  }
	else {
	    cout << "Incompatible arguments for the operator 'LS'" ;
	    exit(1) ;
	  }
	break ;
      case Control::LE :
	if(rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER) {
	    if( atoi(rand1->value().c_str()) <= atoi(rand2->value().c_str()) )
	      result->set_type(Control::TRUE) ;
	    else
	      result->set_type(Control::FALSE) ;
	    break ;
	  }
	else {
	    cout << "Incompatible arguments for the operator 'LE'" ;
	    exit(1) ;
	  }
	break ;
      case Control::EQ :
	if( rand1->type() == Control::STRING && rand2->type() == Control::STRING ) {
	    if( rand1->value().compare(rand2->value()) == 0)
	      result->set_type(Control::TRUE) ;
	    else
	      result->set_type(Control::FALSE) ;
				}
	else if( rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER )  {
	    if( rand1->value() == rand2->value() )
	      result->set_type( Control::TRUE );
	    else
	      result->set_type(Control::FALSE );
	  }
	else if( ( rand1->type() == Control::TRUE || rand1->type() == Control::FALSE ) && ( rand1->type() == Control::TRUE || rand2->type() == Control::FALSE ) ) {
	    if( rand1->type() == rand2->type() )
	      result->set_type( Control::TRUE) ;
	    else
	      result->set_type( Control::FALSE) ;
	  }
	else {
	    cout << "Incompatible arguments for the operator 'EQ'" ;
	    exit(1) ;
	  }
	break ;		
      case Control::NE :
	if( rand1->type() == Control::STRING && rand2->type() == Control::STRING ) {
	    if( rand1->value().compare(rand2->value()) != 0)
	      result->set_type(Control::TRUE) ;
	    else
	      result->set_type(Control::FALSE) ;
				}
	else if( rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER ) {
	    if( rand1->value() != rand2->value() )
	      result->set_type( Control::TRUE );
	    else
	      result->set_type(Control::FALSE );
	  }
	else if( ( rand1->type() == Control::TRUE || rand1->type() == Control::FALSE ) && ( rand1->type() == Control::TRUE || rand2->type() == Control::FALSE ) ) {
	    if( rand1->type() != rand2->type() )
	      result->set_type( Control::TRUE) ;
	    else
	      result->set_type( Control::FALSE) ;
	  }
	else {
	    cout << "Incompatible arguments for the operator 'NE'" ;
	    exit(1) ;
	  }
	break ;
      case Control::OR :
	if( (rand1->type() == Control::TRUE || rand1->type() == Control::FALSE ) &&
	    (rand2->type() == Control::TRUE || rand2->type() == Control::FALSE) ) {
	    bool op1 = ( rand1->type() == Control::TRUE )? true : false ;
	    bool op2 = ( rand2->type() == Control::TRUE )? true : false ;
	    bool res = op1 | op2 ;
	    result->set_type(res? Control::TRUE : Control::FALSE) ;
	  }
	else {
	    cout << "Incompatible arguments for the operator 'OR'" ;
	    exit(1) ;
	  }
	break ;
      case Control::AND_LOGICAL :
	if( (rand1->type() == Control::TRUE || rand1->type() == Control::FALSE ) &&
	    (rand2->type() == Control::TRUE || rand2->type() == Control::FALSE) ) {
	    bool op1 = ( rand1->type() == Control::TRUE )? true : false ;
	    bool op2 = ( rand2->type() == Control::TRUE )? true : false ;
	    bool res = op1 & op2 ;
	    result->set_type(res? Control::TRUE : Control::FALSE) ;
	  }
	else {
	    cout << "Incompatible arguments for the operator 'AND'" ;
	    exit(1) ;
	  }
	break ;
      case Control::ADD :
	if(rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER){
	    result->set_value(itos(atoi(rand1->value().c_str()) + atoi(rand2->value().c_str())));
	    break ;
	  }
	else{
	    cout << "Incompatible arguments for the operator '+'" ;
	    exit(1) ;
	  }
	break ;
	
      case Control::SUBTRACT :
	if(rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER){
	    result->set_value(itos(atoi(rand1->value().c_str()) - atoi(rand2->value().c_str())));
	    break ;
	  }
	else {
	    cout << "Incompatible arguments for the operator '-'" ;
	    exit(1) ;
	  }
	break ;
      case Control::MULTIPLY :
	if(rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER){
	    result->set_value(itos(atoi(rand1->value().c_str()) * atoi(rand2->value().c_str())));
	    break ;
	  }
	else{
	    cout << "Incompatible arguments for the operator '*'" ;
	    exit(1) ;
	  }
	break ;
      case Control::DIVIDE :
	if(rand1->type() == Control::INTEGER && rand2->type() == Control::INTEGER) {
	    result->set_value(itos(atoi(rand1->value().c_str()) / atoi(rand2->value().c_str())));
	    break ;
	  }
	else {
	    cout << "Incompatible arguments for the operator '*'" ;
	    exit(1) ;
	  }
	break ;
      default:
	cout << "UnHandled binary operator , type: " << type << endl;
	throw "UnHandled binary operator ";
      };
    
    _stack.push(result) ;
  };
  void escape_print_string(string print_str){
    //acutally printing the value
    for( int i = 0 ; i < print_str.length() ; i++ ){
	char ch1 = print_str.at(i) ;
	  if( ch1 == '\\'){
	    i++ ;
	    if( i < print_str.length() ){
	      char ch2 = print_str.at(i) ;
	      if( ch2 == 'n' )
		cout << endl;
	      else if( ch2 == 't' )
		cout << "\t" ;
	      else
		cout <<  ch1 << ch2 ;
	    }
	  }
	  else
	    cout << ch1 ;
      }      
      cout.flush();
  };
  void check_type_apply_rator(Control::Type type1, Control::Type type2){
    Control *temp = _stack.top();
    _stack.pop();
    Control *bool_result = new Control((temp->type() == type1 || temp->type() == type2) ? Control::TRUE : Control::FALSE);
    _stack.push(bool_result);
  }
  void check_type_apply_rator(Control::Type type){
    Control *temp = _stack.top();
    _stack.pop();
    Control *bool_result = new Control(temp->type() == type ? Control::TRUE : Control::FALSE);
    _stack.push(bool_result);
  }
  //apply functions CSE Rule 3
  void apply_rator(Control *rator){
    if(rator->_variables.front() == "Print"){
      string print_str = _stack.top()->to_s(); // i dont think this should work .. here it should use the lookup
      escape_print_string(print_str);
      
      _stack.pop();
      _stack.push(new Control(Control::DUMMY));
    }
    else if(rator->_variables.front() == "Order"){
      Control *tuple = _stack.top() ;
      _stack.pop();
      Control *order = new Control() ;
      order->set_type(Control::INTEGER) ;
      if( tuple->type() == Control::TUPLE){
	order->set_value(itos(tuple->_tuples.size())) ;
      }
      else if( tuple->type() == Control::NIL ){
	order->set_value(itos(0)) ;
      }
      else{
	cout <<  "Invalid argument for 'Order'" << endl ;
	exit(1) ;
      }
      _stack.push(order) ;
    }
    else if(rator->_variables.front() == "Isinteger"){
      check_type_apply_rator(Control::INTEGER);
    }
    else if(rator->_variables.front() == "Istruthvalue"){
      check_type_apply_rator(Control::TRUE, Control::FALSE);
    }
    else if(rator->_variables.front() == "Isstring"){
      check_type_apply_rator(Control::STRING);
    }
    else if(rator->_variables.front() == "Istuple"){
      check_type_apply_rator(Control::TUPLE, Control::NIL);
    }
    else if(rator->_variables.front() == "Isfunction"){
      check_type_apply_rator(Control::LAMBDA);
    }
    else if(rator->_variables.front() == "Isdummy"){
      check_type_apply_rator(Control::DUMMY);
    }
    else if(rator->_variables.front() == "Stem"){
      if(_stack.top()->type() == Control::STRING){
	Control *str_cont = new Control(Control::STRING);
	str_cont->set_value(_stack.top()->value().substr(0, 1));
	_stack.pop();
	_stack.push(str_cont);
      }
      else{
	cout << "STEM: Expecting String" << endl;
	exit(1);
      }
    }
    else if(rator->_variables.front() == "Stern"){
      if(_stack.top()->type() == Control::STRING){
	Control *str_cont = new Control(Control::STRING);
	str_cont->set_value(_stack.top()->value().substr(1, _stack.top()->value().length()-1));
	_stack.pop();
	_stack.push(str_cont);
      }
      else{
	cout << "STERN: Expecting String" << endl;
	exit(1);
      }
    }
    else if(rator->_variables.front() == "ItoS"){
      if(_stack.top()->type() == Control::INTEGER){
	Control *str_cont = new Control(Control::STRING);
	str_cont->set_value(_stack.top()->value());
	_stack.pop();
	_stack.push(str_cont);
      }
      else{
	cout << "Itos: Expecting Integer" << endl;
	exit(1);
      };
    }
    else if(rator->_variables.front() == "Conc"){ //could have check for string here
      Control *conc_lambda = new Control(Control::NAME);
      conc_lambda->_variables.push_back("Conclambda");
      conc_lambda->_variables.push_back(_stack.top()->value());
      _stack.pop();
      _stack.push(conc_lambda);
    }
    else if(rator->_variables.front() == "Conclambda"){
      Control *concat_var = new Control(Control::STRING, rator->_variables.at(1)+ _stack.top()->value());
      _stack.pop();
      _stack.push(concat_var);
    }
    else if(rator->_variables.front() == "Null"){
      Control *bool_res = new Control();
      if(_stack.top()->type() == Control::NIL || (_stack.top()->type() == Control::TUPLE && _stack.top()->_tuples.empty()))
	bool_res->set_type(Control::TRUE);
      else
	bool_res->set_type(Control::FALSE);
      _stack.pop();
      _stack.push(bool_res);
    }
    else{
      cout << "Unhandled Rator? value:%" << rator->value() << "%type:" << rator->type() << endl;
      print_control_and_stack();
      throw "Unhandled Rator";
    };
      
  };

  //deltas is the vector of deltas
  vector<Control *> *deltas;// new vector<Control *>;
  Control *root_delta;// = new Control(Control::DELTA, deltas->size());
  
  int env_count;
  vector<Control *> _control;
  stack<Control *> _stack;
  Environment *curr_env;
  Environment *primitive_env;
  stack<Environment *> env_stack;
  map<int, Environment *> env_hash;
  void put_on_stack_pop_from_control(Control *curr_control){
    _control.pop_back(); // vary about this method
    _stack.push(curr_control) ;
  };
  bool is_inbuilt_function(string name){
    if((strcmp (name.c_str(), "Order") == 0) ||
       (strcmp (name.c_str(), "Print") == 0) ||
       (strcmp (name.c_str(), "Isinteger") == 0) ||
       (strcmp (name.c_str(), "Istruthvalue") == 0) ||
       (strcmp (name.c_str(), "Isstring") == 0) ||
       (strcmp (name.c_str(), "Istuple") == 0) ||
       (strcmp (name.c_str(), "Isfunction") == 0) ||
       (strcmp (name.c_str(), "Isdummy") == 0) ||
       (strcmp (name.c_str(), "Stem") == 0) ||
       (strcmp (name.c_str(), "Stern") == 0) ||
       (strcmp (name.c_str(), "Conc") == 0) ||
       (strcmp (name.c_str(), "Conc2") == 0) ||
       (strcmp (name.c_str(), "ItoS") == 0) ||
       (strcmp (name.c_str(), "Null") == 0)){
      return true;
    }
    else 
      return false;
  };
};


void TreeNode::flatten(Control *delta,vector<Control *> *deltas){

  //used to handle the recursive retrieval of delta and to restore the delta after the new delta is handled
  Control *temp_del_ptr = NULL;
  
  vector<string> *variables = NULL;//new vector<string>();
  if(_type == TreeNode::LAMBDA){
    variables = new vector<string>();
    //now fill the variables
    if(lft->_type == TreeNode::IDENTIFIER){
      variables->push_back(lft->_value);
    }
    else if(lft->_type == TreeNode::COMMA){
      TreeNode *temp = lft->lft;
      while(temp!= NULL){
	variables->push_back(temp->_value);
	temp = temp->rgt;
      };
    }
    else{
      cout << "Expected Identifier or Comma, but din't find" << endl;
    };
    //creating new delta
    temp_del_ptr = new Control(Control::DELTA, deltas->size());
    //adding to the deltas list
    deltas->push_back(temp_del_ptr);
    //adding the current lamda to control structure and referencing it to the newly created delta
    delta->add_control(_type, _value, variables, temp_del_ptr, deltas->size());
    
    //donot need to flatten the lft child since these are variable(or) variables
    //flatenning the body of lambda
    lft->rgt->flatten(temp_del_ptr, deltas);
    
    if(rgt!=NULL)
      rgt->flatten(delta, deltas);
  }
  else if(_type == TreeNode::TERNARY){
    Control *delta_then = new Control(Control::DELTA, deltas->size());
    deltas->push_back(delta_then);
    delta->_control_struct->push_back(new Control(Control::DELTA, deltas->size()-1)); //delta then
    if(lft->rgt->_type == TreeNode::TERNARY){
      lft->rgt->flatten(delta_then, deltas);
    }
    else{
      vector<string> *temp_variables = NULL;
      if(lft->rgt->_type == TreeNode::TAU){
	TreeNode *temp = lft->rgt->lft;
	temp_variables = new vector<string>;
	while(temp!= NULL){
	  temp_variables->push_back(temp->_value); // will these be any useful
	  temp = temp->rgt;
	}
      }
      delta_then->add_control(lft->rgt->_type, lft->rgt->_value, temp_variables, delta_then, deltas->size());
      if(lft->rgt->lft != NULL)
	lft->rgt->lft->flatten(delta_then, deltas);
    }
    
    Control *delta_else = new Control(Control::DELTA, deltas->size());
    deltas->push_back(delta_else);
    delta->_control_struct->push_back(new Control(Control::DELTA, deltas->size()-1)); //delta else

    if(lft->rgt->rgt->_type == TreeNode::TERNARY){
      lft->rgt->rgt->flatten(delta_else, deltas);
    }
    else{
      vector<string> *temp_variables = NULL;
      if(lft->rgt->rgt->_type == TreeNode::TAU){
	TreeNode *temp = lft->rgt->rgt->lft;
	temp_variables = new vector<string>;
	while(temp!= NULL){
	  temp_variables->push_back(temp->_value); // will these be any useful
	  temp = temp->rgt;
	}
      }
      delta_else->add_control(lft->rgt->rgt->_type, lft->rgt->rgt->_value, temp_variables, delta_else, deltas->size());
      if(lft->rgt->rgt->lft != NULL)
	lft->rgt->rgt->lft->flatten(delta_else, deltas);      
    };
    
    Control *beta = new Control(Control::BETA);
    delta->_control_struct->push_back(new Control(Control::BETA, "beta"));
    delta->add_control(lft->_type, lft->_value, NULL, NULL, deltas->size());
    if(lft->lft != NULL)
      lft->lft->flatten(delta, deltas);
  }
  else{
    //checking for speacial cases like TAU
    if(_type == TreeNode::TAU){
      variables = new vector<string>();
      TreeNode *temp = lft;
      while(temp!= NULL){
	variables->push_back(temp->_value);//will these be any useful ?
	temp = temp->rgt;
      };
    };

    //create a new control and flatten urself. in case of non LAMBDA node
    delta->add_control(_type, _value, variables, temp_del_ptr, deltas->size());
    //flatten you left kid and then the right kid
    if(lft !=NULL){
      lft->flatten(delta, deltas);
      //if(lft->rgt != NULL){
      //	lft->rgt->flatten(delta, deltas);
      //}
    };
    if(rgt!=NULL){
      rgt->flatten(delta,deltas);
    };
  };
};

void Control::add_control(int type, string value, vector<string> *variables, Control* del_ptr, int deltas_size){ // cheating this type had to be Treenode::Type 
    //has to store appropriate control depeneding on the type and value of the tree node
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
  case TreeNode::PLUS:
    temp = new Control(Control::ADD, value);
    break;
  case TreeNode::MINUS:
    temp = new Control(Control::SUBTRACT, value);
    break;
  case TreeNode::DIVIDE:
    temp = new Control(Control::DIVIDE, value);
    break;
  case TreeNode::GAMMA:
    temp = new Control(Control::GAMMA, value);
    break;
  case TreeNode::IDENTIFIER:
    temp = new Control(value, Control::NAME);
    break;
  case TreeNode::STRING:
    //cout << " Value is : "<< value << endl;
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
    cout << "UnHandled TreeNode/Control found!?, TreeNode value:" << value << " type:" << type << endl;
    throw "UnHandled Control found!?";
    break;
  };
  //cout << temp->to_s()<< "control stuct size:" <<  << endl;
  _control_struct->push_back(temp);
};


//should this be avaiblable for the delta node ?
  void Control::pretty_print(){
    if(_type!=Control::DELTA){
      cout << "Not a delta node, cannot pretty print" << endl;
      throw "pretty_print called on delta node";
    };
    //cout << "size of control struct: " << _control_struct->size() << endl;
    cout << to_s() << " ";
    for(int i=0; i< _control_struct->size(); i++){
      cout << _control_struct->at(i)->to_s() << " " ;
    };
    cout << endl;
  };
