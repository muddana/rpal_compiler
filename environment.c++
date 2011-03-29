#include<map>

class Environment{
public:
  void assign_parent(Environment *parent){
    _parent = parent;
  };

  Control* lookup(string symbol){
    map<string, Control *>::iterator it;
    it = symbol_table.find(symbol);
    if(symbol_table.end() == it)
       return _parent->lookup(symbol);
    else
      return (*it).second;
  };
  Environment(int id): _id(id){
  };
  int id() const{
    return _id;
  };
  void pretty_print(){
    map<string, Control *>::iterator it;
    for ( it=symbol_table.begin() ; symbol_table.end() != it; it++ )
      cout << (*it).first << " => " << (*it).second->to_s() << endl;
  };
  map<string, Control *> symbol_table;
private:
  const int _id;
  Environment *_parent; //should have all the primitive env data like common functions like -, + etc.
};
