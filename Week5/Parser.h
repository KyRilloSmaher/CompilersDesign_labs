#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Scanner.h"

using namespace std;
/*
S→bA
𝐴→𝑎𝐴∣ 𝜖
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
            StartSymbol();
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
    void StartSymbol() {
        Match("<B>");
        A();
        if (LookaheadToken == "") { 
            cout << "Parsing successful!" << endl;
        } else {
            cout << "Fail to parse!" << endl;
            exit(1);
        }
    }

    void A() {
       while(LookaheadToken == "<A>") {
            Match("<A>");
        }
    }
#pragma endregion
};

#endif // PARSER_H
