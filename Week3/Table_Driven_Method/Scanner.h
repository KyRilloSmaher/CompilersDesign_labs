#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <algorithm>
#include <cctype>
#include "Token.h"
using namespace std;

class Scanner
{
private:
#pragma region Fields
    ifstream input_file;
    ofstream output_file;
    string current_state;
    set<string> keywords;
    vector<string> states;
    set<string> finalstates;
    string initialState;
    string Errorstate;
    vector<Token> tokens;
    vector<pair<pair<string, string>, string>> transitionTable;

#pragma endregion

#pragma region Helping methods
    void split(const string &line, char delimiter, vector<string> &target)
    {
        int linelength = line.length();
        string temp = "";
        for (int i = 0; i < linelength; i++)
        {
            if (line[i] != delimiter)
                temp += line[i];
            else
            {
                target.push_back(temp);
                temp = "";
            }
        }
        target.push_back(temp); // for last token
    }

    string classifyCharacter(char c)
    {
        if (isdigit(c))
            return "number";
        if (isalpha(c))
            return "char";
        if (c == '(' || c == ')' || c == ',')
            return string(1, c);
        return "other";
    }
#pragma endregion

public:
#pragma region Constructours
    Scanner(const string &input_file_path, const string &output_file_path)
    {
        current_state = "q0";
        input_file.open(input_file_path);
        output_file.open(output_file_path);

        if (!input_file.is_open())
        {
            cerr << "Error: Unable to open input file: " << input_file_path << endl;
            exit(1);
        }
        if (!output_file.is_open())
        {
            cerr << "Error: Unable to open output file: " << output_file_path << endl;
            exit(1);
        }

        LoadKeywords();
        LoadStates();
        LoadTransitionTable();
        current_state = initialState;
    }
#pragma endregion

#pragma region Destructor
    ~Scanner()
    {
        input_file.close();
        output_file.close();
    }
#pragma endregion

#pragma region Main Methods of Scanner

    void LoadKeywords()
    {
        ifstream keywordFile("keywords.txt");
        string keyword;
        while (getline(keywordFile, keyword))
        {
            keywords.insert(keyword);
        }
        keywordFile.close();
        // Debug output
        cout << "Language Keywords: ";
        for (const auto &keyword : keywords)
        {
            cout << keyword << endl;
        }
    }

    void LoadStates()
    {
        ifstream stateFile("automaton.txt");
        string statesLine;
        // get all the states
        if (getline(stateFile, statesLine))
        {
            split(statesLine, ',', states);
        }

        // get initial state
        if (getline(stateFile, statesLine))
        {
            initialState = statesLine;
        }

        // get final states
        vector<string> finalStates;
        if (getline(stateFile, statesLine))
        {
            split(statesLine, ',', finalStates);
            // put them in set
            for (const auto &state : finalStates)
            {
                finalstates.insert(state);
            }
        }

        // get error state
        if (getline(stateFile, statesLine))
        {
            Errorstate = statesLine;
        }

        stateFile.close();
        // Debug output
        cout << "States: ";
        for (const auto &state : states)
        {
            cout << state << ", ";
        }
        cout << endl;
        cout << "Initial State: " << initialState << endl;
        cout << "Final States: ";
        for (const auto &state : finalstates)
        {
            cout << state << ", ";
        }
        cout << endl;
        cout << "Error State: " << Errorstate << endl;
    }

    void LoadTransitionTable()
    {
        ifstream transitionFile("transition.txt");
        string line;
        // read transition table from file line by line with seperating each line into three parts currentstate & input and gotostate using delimeter # .
        while (getline(transitionFile, line))
        {
            vector<string> currentstate_input_gotostate;
            split(line, '#', currentstate_input_gotostate);
            // adding each row to the table
            transitionTable.push_back({{currentstate_input_gotostate[0], currentstate_input_gotostate[1]}, currentstate_input_gotostate[2]});
        }

        transitionFile.close();
        // Debug output
        cout << "Transition Table: " << endl;
        cout << "----------------------------" << endl;
        cout << "current_state | INPUT | GOTO" << endl;
        cout << "----------------------------" << endl;
        for (const auto &entry : transitionTable)
        {
            cout << entry.first.first << "      | " << entry.first.second << "  --->    " << entry.second << endl;
        }
    }

    string getTransition(const string &from, const string &chr)
    {
        for (const auto &entry : transitionTable)
        {
            if (entry.first.first == from && entry.first.second == chr)
            {
                return entry.second;
            }
        }
        return "";
    }

    string getTokenType(const string &tkn)
    {
        if (keywords.find(tkn) != keywords.end())
        {
            string upperTkn = tkn;
            transform(upperTkn.begin(), upperTkn.end(), upperTkn.begin(), ::toupper);
            return "<" + upperTkn + ">";
        }
        if (isdigit(tkn[0]))
            return "<NUMBER>";
        if (tkn == "(")
            return "<LPAREN>";
        if (tkn == ")")
            return "<RPAREN>";
        if (tkn == ",")
            return "<COMMA>";
        return "<UNKNOWN>";
    }

    void scan()
    {
        char chr;
        string str = "";
        while (input_file.peek() != EOF && current_state != Errorstate)
        {
            chr = input_file.get();
            if (isspace(chr))
            {
                if (!str.empty())
                {
                    string tokenType = getTokenType(str);
                    tokens.push_back(Token(tokenType, str, current_state));
                    str = "";
                }
                continue;
            }
            else if (isalpha(chr))
            {
                str += chr;
                string nextState = getTransition(current_state, classifyCharacter(chr));
                if (nextState.empty())
                {
                    current_state = Errorstate;
                }
                else
                    current_state = nextState;
            }
            else if (isdigit(chr))
            {
                str += chr;
                string nextState = getTransition(current_state, classifyCharacter(chr));
                if (nextState.empty())
                {
                    current_state = Errorstate;
                }
                else
                    current_state = nextState;
            }
            else if (chr == '(' || chr == ')' || chr == ',')
            {
                if (!str.empty())
                {
                    string tokenType = getTokenType(str);
                    tokens.push_back(Token(tokenType, str, current_state));
                    str = "";
                }

                str += chr;
                current_state = getTransition(current_state, classifyCharacter(chr));
                tokens.push_back(Token(getTokenType(classifyCharacter(chr)), str, current_state));
                str = "";
            }
        }
        // write tokens to tokens.txt
        for (const auto &token : tokens)
        {
            output_file << "Token: " << token.TokenType << " | Value: " << token.TokenValue << " | State: " << token.State << endl;
        }

             (current_state == Errorstate) ?
             output_file << "THERE IS A Lexical Error ..." << endl:
             (finalstates.find(current_state) != finalstates.end())?
              output_file << "THE PROGRAM IS ACCEPTED :) ..." :
              output_file << "THE FUCKING PROGRAM IS REJECTED >:( " ;
        
    }
};

#pragma endregion

#endif
