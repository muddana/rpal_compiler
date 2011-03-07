class Token{
public:
  Token(): _value(""), _type(0){
  };
  Token(string token_value):_value(token_value), _type(0){
  };
  Token(int type_of_tok): _value(""),_type(type_of_tok){
  };
  Token(int type_of_tok, string token_value): _value(token_value),_type(type_of_tok){
  };
  enum Types { IDENTIFIER = 0,
	       INTEGER = 1,
	       OPERATOR = 2,
               STRING =  3,
	       DELETE = 4,
	       PUNCTION = 5,
	       ENDOFFILE = 6
              };
  int type() const{
    return _type;
  };
  string value() const{
    return _value;
  };
private:
  const string _value;
  const int _type;
};
