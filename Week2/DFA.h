#ifndef _DFA
#define _DFA

#include<iostream>
#include<string>
#include "States.h"
using namespace std;
class DFA
{
private:
    /* data */
    State currentState;
public:
    DFA(/* args */){}
    ~DFA(){}
    bool check(string str){
      int len = str.length();
      if(len == 0) return false;
      currentState = A ;
      int i = 0;
      while(i<len){
        switch(currentState){
            case A:
                    if(str[i] == 'a') currentState = B;
                    else if(str[i] == 'b') currentState = Y;
                    else return false;
                    break;
            case B:
                    if(str[i] == 'a') currentState = B;
                    else if(str[i] == 'b') currentState = C;
                    else return false;
                    break;
            case C:
                if(str[i] == 'b') currentState = R;
                else if(str[i] == 'a') currentState = B;
                else return false;
                break;
            case R:
                if(str[i] == 'a' || str[i] == 'b') currentState = R;
                else return false;
                break;
                
            case Y:
                if(str[i] == 'b') currentState = Y;
                else if(str[i] == 'a') currentState = B;
                else return false;
                break;
            default:
               return false;
        }
        i++;
      }
      return currentState == Y || currentState == B || currentState ==C;
    }
};

#endif
