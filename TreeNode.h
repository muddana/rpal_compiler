
//
// C++ Implementation: test
//
// Description: TreeNode datastructure for implementing AST Tree
//
//
// Author: srinivas muddana, <smuddana@cise.ufl.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
//first child next sibling notation of a n-ry tree
  class TreeNode{
  public:
    //void flatten(Control *, vector<Control *> *);

    enum Type {
      LAMBDA = 1 ,
      WHERE = 2 ,
      TAU = 3 ,
      AUG = 4 ,
      TERNARY = 5 ,
      OR = 6 ,
      AND_LOGICAL = 7 ,
      NOT = 8 ,
      GR = 9 ,
      GE = 10 ,
      LS = 11 ,
      LE = 12 ,
      EQ = 13 ,
      NE = 14 ,
      ADD = 15 ,
      SUBTRACT = 16 ,
      NEG = 17 ,
      MULTIPLY = 18 ,
      DIVIDE = 19 ,
      POWER = 20 ,
      AT = 21 ,
      GAMMA = 22 ,
      TRUE = 23 ,
      FALSE = 24 ,
      NIL = 25 ,
      DUMMY = 26 ,
      WITHIN = 27 ,
      AND = 28 ,
      REC= 29 ,
      BINDING = 30 ,
      FCN_FORM = 31 ,
      EMPTY_BRACKET = 32 ,
      COMMA = 33 ,
      LET = 34 ,
      IDENTIFIER = 35 ,
      STRING = 36 ,
      INTEGER = 37,
      YSTAR = 38
    };
    
    TreeNode *lft,*rgt;
    TreeNode(string val, Type type): _value(val) , _type(type){
      _init_lft_rgt();
    };
    TreeNode(Type type): _type(type){
      _init_lft_rgt();
    };
    //pre-order treversal first root then the left and right child recursively
    void pretty_print() const;

    string to_s() const;

    void standardize();
    void addChild(TreeNode *child);

    string value() const{
      return _value;
    };
    Type type() const{
      return _type;
    };
    string _value;
    Type _type;
  private:
    void _standardize(){
      TreeNode *p, *e, *e1, *e2, *x, *x1, *x2, *n, *temp, *new_temp;

      if(NULL != lft){
	lft->standardize();
      };
      if(NULL != rgt){
	rgt->standardize();
      };
      switch(_type){
      case LET :
	if(BINDING != lft->_type){
	  throw RpalError(RpalError::INTERNAL, "Subtree not standardised: case LET");
	}
	_type = GAMMA;
	lft->_type = LAMBDA;
	p = lft->rgt;
	lft->rgt = lft->lft->rgt;
	lft->lft->rgt = p;
	break;
      case WHERE:
	if(BINDING != lft->rgt->_type){
	  throw RpalError(RpalError::INTERNAL,"Subtree not standardised: case WHERE");
	};
	_type = GAMMA;
	p = lft;
	x = lft->rgt->lft;
	e = lft->rgt->lft->rgt;
	p->rgt = NULL; // dissociating P with =
	x->rgt = NULL; // dissociating X with E
	lft = new TreeNode(LAMBDA);
	lft->rgt = e;
	lft->lft = x;
	x->rgt = p;
	break;
      case WITHIN:
	if(BINDING != lft->_type || BINDING != lft->rgt->_type){
	  throw RpalError(RpalError::INTERNAL,"Subtree not standardised: case WITHIN");
	};
	_type = BINDING;
	x1 = lft->lft;//x1 and its next siblling(rgt) is E1
	x2 = lft->rgt->lft;
	//swapping e1 and e2
	e1 = x1->rgt;
	x1->rgt = x2->rgt;
	//end swapping e1 and e2
	x2->rgt = new TreeNode(GAMMA);
	x2->rgt->lft = new TreeNode(LAMBDA);
	x2->rgt->lft->rgt = e1; // the new lambda's right
	x2->rgt->lft->lft = x1; // the new lambda's left has now x1 who's first sibling is e2
	lft = x2;
	break;
      case REC:
	if(BINDING != lft->_type){
	  throw RpalError(RpalError::INTERNAL,"Subtree not standardised: case REC");
	};
	_type = BINDING;
	x = lft->lft;
	e = lft->lft->rgt;
	x->rgt = NULL;
	lft = x; //removing the reference to the old = node
	temp = new TreeNode(GAMMA);
	x->rgt = temp;
	temp-> lft = new TreeNode(YSTAR);
	temp->lft->rgt = new TreeNode(LAMBDA);
	temp->lft->rgt->lft = new TreeNode(x->_value, x->_type); // use copy constructor later ?? what is a copy constructor?
	temp->lft->rgt->lft->rgt = e;
	break;
      case FCN_FORM:
	_type = BINDING;
	temp = lft;
	while(temp->rgt->rgt != NULL){
	  new_temp = new TreeNode(LAMBDA);
	  new_temp->lft = temp->rgt; //temp->rgt is the current V therefore temp is the previous V
	  temp->rgt = new_temp;
	  temp = new_temp->lft;	  
	};
	break;
      case LAMBDA:
	temp = lft;
	while(temp->rgt->rgt != NULL){
	  new_temp = new TreeNode(LAMBDA);
	  new_temp->lft = temp->rgt; //temp->rgt is the current V therefore temp is the previous V
	  temp->rgt = new_temp;
	  temp = temp->rgt;	  
	};
	break;
      case AND:
	_type = BINDING;
	temp = lft;
	lft = NULL;
	lft = new TreeNode(COMMA);
	lft->rgt = new TreeNode(TAU);
	while(temp!=NULL){
	  lft->rgt->addChild(temp->lft->rgt);//adding children to the tau node
	  temp->lft->rgt = NULL;
	  lft->addChild(temp->lft);//adding children to the comma node
	  temp = temp->rgt;
	};
	break;
	case AT:
	_type = GAMMA;	
	e1 = lft;
	n = e1->rgt;
	e2 = e1->rgt->rgt;
	lft = new TreeNode(GAMMA);
	lft->rgt = e2;
	lft->lft = n;
	n->rgt = NULL;
	lft->lft->rgt = e1;
	e1->rgt = NULL;
	break;
      };
    };

    void _pretty_print(int level) const{
      for(int i=0; i <level; i++){
	cout << ".";
      }
      cout << to_s() << " " << endl;
      if(NULL != lft)
	lft->_pretty_print(level + 1);
      if(NULL != rgt)
	rgt->_pretty_print(level);
    };


    void _init_lft_rgt(){
      lft = NULL;
      rgt = NULL;
    };
  };
