//
// C++ Implementation: test
//
// Description: helper for the project with simple functions like toenize etc.
//
//
// Author: srinivas muddana, <smuddana@cise.ufl.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <algorithm>
#include <math.h>
using namespace std;

void tokenize(string str, char token, vector<string> *cont){
  string temp;
  for(string::iterator it = str.begin();it <= str.end(); it++){
    if(token == *it){
      if(!temp.empty()){
	cont->push_back(temp);
      }
	temp = "";
    }
    else{
      temp += *it;
    } 
  }
  if(!temp.empty()){
    cont->push_back(temp);
  }
};
//test cases for tokenize go here
void test_tokenize(){
  vector<string> test;
  tokenize(",test, asfds,sdf asdf,asfa ,", ',', &test);
  for(vector<string>::iterator it=test.begin(); it < test.end(); it++)
    cout << *it << " ";
};

template<class T>
int find_in_vector(vector<T> &vec, T primitive_val){
  int index = -1;
  for(int i=0;i <= vec.size();i++)
    {
      if(vec.at(i)== primitive_val)
	{
	  index = i;
	  break;
	}
    }
  return index;
}

/*string itos(int i)	// convert int to string
{
  stringstream s;
  s << i;
  return s.str();
  }*/

template <class T>
string itos(T i)	// convert int to string
{
  stringstream s;
  s << i;
  return s.str();
}


