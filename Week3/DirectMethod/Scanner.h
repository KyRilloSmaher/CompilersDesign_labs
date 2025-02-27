#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include "Token.h"
#include "states.h"

using namespace std;

class Scanner {
private:
    ifstream input_file;
    ofstream output_file;
    State current_state;
    vector<Token> tokens;
    unordered_map<string, string> tokenMap;

public:
    Scanner(const string &input_file_path, const string &output_file_path) {
        current_state = q0;
        input_file.open(input_file_path);
        output_file.open(output_file_path);

        if (!input_file.is_open()) {
            cerr << "Error: Unable to open input file: " << input_file_path << endl;
            exit(1);
        }
        if (!output_file.is_open()) {
            cerr << "Error: Unable to open output file: " << output_file_path << endl;
            exit(1);
        }

        tokenMap = {
            {"(", "<LPAREN>"}, {")", "<RPAREN>"},
            {",", "<COMMA>"}, {"num", "<NUMBER>"},
            {"red", "<RED>"}, {"blue", "<BLUE>"},
            {"green", "<GREEN>"}, {"white", "<WHITE>"},
            {"yellow", "<YELLOW>"}
        };
    }

    ~Scanner() {
        input_file.close();
        output_file.close();
    }

    string getTokenType(const string &tkn) {
        return (tokenMap.find(tkn) != tokenMap.end()) ? tokenMap[tkn] : "<UNKNOWN>";
    }

    bool isValidColor(const string& str) {
        return tokenMap.find(str) != tokenMap.end();
    }

    void scan() {
        char chr;
        string str = "";
        
        while (input_file.peek() != EOF && current_state != q7) {
            if (input_file.eof()) break;

            switch (current_state) {
                case q0: {
                    chr = input_file.get();
                    if (isspace(chr)) continue;
                    else if (chr == '(') {
                        current_state = q1;
                        tokens.push_back(Token(getTokenType("("), "(", to_string(current_state)));
                    } else {
                        handleError(chr);
                    }
                    break;
                }

                case q1: {
                    chr = input_file.get();
                    if (isspace(chr)) continue;
                    else if (isdigit(chr)) {
                        str += chr;
                        current_state = q2;
                    } else {
                        handleError(chr);
                    }
                    break;
                }

                case q2: {
                    chr = input_file.get();
                    if (isspace(chr) || chr == ',') {
                        tokens.push_back(Token(getTokenType("num"), str, to_string(current_state)));
                        str = "";
                        if (chr == ',') {
                            current_state = q3;
                            tokens.push_back(Token(getTokenType(","), ",", to_string(current_state)));
                        }  else {
                            current_state = q0;
                        }
                    } else if (isdigit(chr)) {
                        str += chr;
                    } else {
                        handleError(chr);
                    }
                    break;
                }

                case q3: {
                    chr = input_file.get();
                    if (isspace(chr)) continue;
                    else if (isalpha(chr)) {
                        str += chr;
                        current_state = q5;
                    } else if (isdigit(chr)) {
                        str += chr;
                        current_state = q4;
                    } else {
                        handleError(chr);
                    }
                    break;
                }

                case q4: {
                    chr = input_file.get();
                    if (isspace(chr) || chr == ')') {
                        tokens.push_back(Token(getTokenType("num"), str, to_string(current_state)));
                        str = "";
                        if (chr == ')') {
                            current_state = q6;
                            tokens.push_back(Token(getTokenType(")"), ")", to_string(current_state)));
                        } else {
                            current_state = q0;
                        }
                    } else if (isdigit(chr)) {
                        str += chr;
                    } else {
                        handleError(chr);
                    }
                    break;
                }

                case q5: {
                    chr = input_file.get();
                    if (isspace(chr) || chr == ')') {
                        if (isValidColor(str)) {
                            tokens.push_back(Token(getTokenType(str), str, to_string(current_state)));
                        } else {
                            handleError(str);
                        }
                        str = "";
                        if (chr == ')') {
                            current_state = q6;
                            tokens.push_back(Token(getTokenType(")"), ")", to_string(current_state)));
                        } else {
                            current_state = q0;
                        }
                    } else if (isalpha(chr)) {
                        str += chr;
                    } else {
                        handleError(chr);
                    }
                    break;
                }

                case q6: {
                    chr = input_file.get();
                    if (isspace(chr)) continue;
                    else if (chr == ',') {
                        current_state = q0;
                        tokens.push_back(Token(getTokenType(","), ",", to_string(current_state)));
                    } else {
                        handleError(chr);
                    }
                    break;
                }

                case q7:
                default:
                    break;
            }
        }

        // Output tokens to the file
        for (const auto& token : tokens) {
            output_file << "Token: " << token.TokenType
                        << " | Value: " << token.TokenValue
                        << " | Current State: " << token.State << endl;
        }
    }

private:
    void handleError(char chr) {
        cerr << "Error: Unexpected character '" << chr << "' in input.\n";
        exit(1);
    }

    void handleError(const string& str) {
        cerr << "Error: Unexpected token '" << str << "' in input.\n";
        exit(1);
    }
};

#endif
