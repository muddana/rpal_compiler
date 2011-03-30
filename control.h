class Control{
public:
  void add_control(int , string,  vector<string> *, Control*, int);  // why not pass the whole node(pointer) itself
  void pretty_print();

  enum Type{
    ENV = 1 ,
    DELTA = 2 ,
    IDENTIFIER = 3 ,
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
  string to_s();

  Control( Control *cntrl);
  
  Control(Control::Type type, int index);
  
  Control(Control::Type type,vector<string> *variables, Control *del_ptr, int delta_index);

  //For TAU and ENV
  Control(Control::Type type, int index, bool watever);

  //make sure this is for NAME nodes only
  Control(string var_value, Control::Type type );
  
  Control(Control::Type type, string value);
  
  Control();

  //make sure this is used for AUG && NIL && YSTAR
  Control(Control::Type type);

  vector<Control *> *_control_struct;

  Type type();

  string value();

  int index();

  void set_linked_env_id(int id);

  int linked_env_id();

  void set_type(Type type);

  void set_value(string value);

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
