class Rpal{
public:
  Rpal(RpalParser* parser): _parser(parser){
    fetchAST();
  };
  
  void printAST(){
    _astStack->top()->pretty_print();
  };
  void printST(){
    //have to change this to make use of standardizer to consume ast and not other way around
    _astStack->top()->standardize();
    _astStack->top()->pretty_print();
  };
  void execute(){
    _astStack->top()->standardize();
    _cseMachine.run(_astStack->top());
  };
private:
  void fetchAST(){
    _astStack = _parser->parse();
  };
  RpalParser* _parser;
  stack<TreeNode *> *_astStack;
  CSE _cseMachine;
};
