#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Scanner.h"

using namespace std;
/*

    Exp -> Term Expr
    Expr ->  + Term Expr | - Term Expr | e
    Term -> Factor Termp
    Termp ->  * Factor Termp |  / Factor Termp | e  
    Factor -> ( Exp ) | id
*/
class Parser {
private:
    ifstream input_file;
    ofstream output_file;
    vector<string> Tokens;
    string LookaheadToken;
    int PointerToToken = 0;

public:
    #pragma region  ==== Constructor ====
    Parser(const string &Scanner_input_file_path, const string &Scanner_output_file_path, const string &Parser_Output_file) {
        Scanner sc(Scanner_input_file_path, Scanner_output_file_path);
        sc.scan();

        input_file.open(Scanner_output_file_path);
        output_file.open(Parser_Output_file);
        if (!output_file.is_open()) {
            cerr << "Error: Unable to open output file: " << Parser_Output_file << endl;
            exit(1);
        }

        ReadInputFile();
        if (!Tokens.empty()) {
            LookaheadToken = Tokens[PointerToToken];
            EXP();
        } else {
            cerr << "Error: No tokens to parse!" << endl;
            exit(1);
        }

    }
    #pragma endregion



    #pragma region  ==== Destructors====
    ~Parser() {
        output_file.close();
    }
    #pragma endregion



    // ==== Read Tokens from File ====
    void ReadInputFile() {
        string line;
        string token;
        while (getline(input_file, line)) {
            token = split(line ,'|');
            Tokens.push_back(token);
        }
        input_file.close();
    }
    string split(const string &line, char delimiter) {
        string temp;
        for (char ch : line) {
            if (ch != delimiter) {
                temp += ch;
            } else {
                return temp;
            }
        }
        return temp; // If there's no delimiter, return the full line
    }


   #pragma region  ==== Print Tokens for Debugging ====
     void PrintTokens() {
        for (const auto &token : Tokens) {
            cout << "Token: " << token << endl;
        }
    }
    #pragma endregion


   #pragma region  ==== Match Function ====
    void Match(const string &matchstring) {
        if (LookaheadToken == matchstring) {
            output_file << "Matched token: " << matchstring << endl;
            if (PointerToToken + 1 < Tokens.size()) {
                LookaheadToken = Tokens[++PointerToToken];
            } else {
                LookaheadToken = ""; // Mark end-of-input
            }
        } else {
            cerr << "Error: Unexpected token: " << LookaheadToken << ", expected: " << matchstring << endl;
            exit(1);
        }
    }
#pragma endregion


 #pragma region  ==== Parsing Rules ====
 //Exp -> Term Expr   
 void EXP() {
        Term();
        EXPR();
    }
//   Term -> Factor Termp
    void Term() {
        Factor();
        TERMP();
    }
//  Expr ->  + Term Expr | - Term Expr | e
void EXPR() {
    if (LookaheadToken == "<+>" || LookaheadToken == "<->") {
        Match(LookaheadToken);
        Term();
        EXPR();
    }
}

    // Factor -> ( Exp ) | id
    void Factor(){
        if (LookaheadToken == "<(>") {
            Match("<(>");
            EXP();
            Match("<)>");
        }
        else if (LookaheadToken == "<id>") {
            Match("<id>");
        }
        else {
            cerr << "Error: Unexpected token: " << LookaheadToken << ", expected: ( or id" << endl;
            exit(1);
        }
    }
    // Termp ->  * Factor Termp |  / Factor Termp | e  
    void TERMP() {
        if (LookaheadToken == "<*>") {
            Match("<*>");
            Factor();
            TERMP();
        }
        else if (LookaheadToken == "</>") {
            Match("</>");
            Factor();
            TERMP();
        }
    }
#pragma endregion
};

#endif // PARSER_H
