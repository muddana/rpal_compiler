class RpalError : public exception{
 public:
 RpalError( int type, string message): _message(message), _type(type){
    
  };
  
  virtual const char* what() const throw()
  {
    cout  << _message << endl;
    return buildErrorStr();
  }
  const static int  LEXER = 1;
  const static int PARSER = 2;
  const static int INTERNAL = 3;
  const static int INTERPRETER = 4;

  ~RpalError () throw (){};

 private:
  const char* buildErrorStr() const throw(){
    switch(_type){
    case RpalError::LEXER:
      return string("LEXER Error: " + _message).c_str();
    case RpalError::PARSER:
      return string("PARSER Error: " + _message).c_str();
    case RpalError::INTERPRETER:
      return string("INTERPRETER Error: " + _message).c_str();
    case RpalError::INTERNAL:
      return string("INTERNAL Error: " + _message).c_str();
    default:
      return "UNKNOWN ERROR";
    };
  };
  const string _message;
  const int _type;
};
