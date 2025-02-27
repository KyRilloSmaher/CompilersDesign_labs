#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

struct Token {
    string TokenType;
    string TokenValue;
    string State;

    Token(string type, string value, string state) {
        TokenType = type;
        TokenValue = value;
        State = state;
    }
};

#endif


