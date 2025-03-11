#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

struct Token {
    string TokenType;
    string State;

    Token(string type,  string state) {
        TokenType = type;
        State = state;
    }
};

#endif


