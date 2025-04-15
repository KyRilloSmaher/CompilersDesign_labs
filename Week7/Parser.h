#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Scanner.h"
using namespace std;
/*

    Exp -> Term Expr
    Expr ->  + Term Expr | - Term Expr | e
    Term -> Factor Termp
    Termp ->  * Factor Termp |  / Factor Termp | e  
    Factor -> ( Exp ) | <id>
*/
class LL1Parser {
private:
    ifstream input_file;
    ofstream output_file;
    vector<string> Tokens;
    set<string> keywords;
    unordered_multimap<string, string> Grammar;
    unordered_map<string,set<char>> FirstSets;
    unordered_map<string,set<char>> FollowSets;
    map<pair<string, char>, string> parsingTable;
public:
    #pragma region  ==== Constructor ====
    LL1Parser(const string &Scanner_input_file_path, const string &Scanner_output_file_path, const string &Parser_Output_file) {
        Scanner sc(Scanner_input_file_path, Scanner_output_file_path);
        sc.scan();
        keywords = sc.getkeywords();
        input_file.open(Scanner_output_file_path);
        output_file.open(Parser_Output_file);
        if (!output_file.is_open()) {
            cerr << "Error: Unable to open output file: " << Parser_Output_file << endl;
            exit(1);
        }

        GetAllTokens();
        if (!Tokens.empty()) {
            ReadGrammar();
            CalculateFirstSets();
            CalculateFollowSets();
            ConstructParsingTable();
            StartParsing();

        } else {
            cerr << "Error: No tokens to parse!" << endl;
            exit(1);
        }

    }
    #pragma endregion
    #pragma region  ==== Destructors====
    ~LL1Parser() {
        output_file.close();
    }
    #pragma endregion

    pair<string,string> split(const string &line, char delimiter) {
        string temp;
        for (int i = 0 ; i<line.length() ; i++) {
            char ch = line[i];
            if (ch != delimiter) {
                temp += ch;
            } else {
               break;
            }
        }
        return pair(temp,line.substr(temp.length() + 1));
    }

    // ==== Read Tokens from File ====
    void GetAllTokens() {
        string line;
        string token;
        while (getline(input_file, line)) {
            Tokens.push_back(line);
        }
        input_file.close();
    }

    // ==== Read Grammer from File ====
    void ReadGrammar() {
        ifstream Grammer_file = ifstream("Grammer.txt");
        string line;
        string token;
        while (getline(Grammer_file, line)) {
             pair<string,string>production= split(line, '@');
            Grammar.emplace(production.first, production.second);
        }
        input_file.close();
        // Debug output
        cout << "Grammar: " << endl;
        for (const auto &pair : Grammar) {
            cout << pair.first << " -> " << pair.second << endl;
        }
    }
    // ==== Calculate the First set for each production ====
    void CalculateFirstSets() {
       for (const auto &pair : Grammar) {
            string non_terminal = pair.first;
            string production = pair.second;
            set<char> first_set = CalculateFirst(non_terminal, production);
            // Check if the non-terminal is already in the FirstSets map to make union
            if (FirstSets.find(non_terminal)!=FirstSets.end()) {
                FirstSets[non_terminal].insert(first_set.begin(), first_set.end());
            }
            else FirstSets[non_terminal] = first_set;
        }
        // Debug output
        cout << "First Sets: " << endl;
        for (const auto &pair : FirstSets) {
            cout << pair.first << " -> ";
            for (char ch : pair.second) {
                cout << ch << " ";
            }
            cout << endl;
        }
    }
    set<char> CalculateFirst(const string &non_terminal, const string &production) {
        set<char> first_set;
        if (production[0] == 'e') { // Epsilon production
            first_set.insert('e');
            return first_set;
        }
        else if (production[0] == '<') { // Non-terminal
            string nonTerminal = "<";
            int i = 1;
            while (i < production.size() && production[i]!= '>') {
                nonTerminal += production[i];
                i++;
            }
            nonTerminal += '>';
            if (FirstSets.find(nonTerminal)!=FirstSets.end()) {


                return FirstSets[nonTerminal];
            }
            else
            {
                string newproduction = Grammar.find(nonTerminal)->second;
                first_set = CalculateFirst(nonTerminal, newproduction);
                FirstSets[nonTerminal] = first_set;
                return first_set;
            }
            
        }
        else { // Terminal
            first_set.insert(production[0]);
            return first_set;
        }
    }
 // ==== Calculate the Follow set for each production ======

    void CalculateFollowSets(){
                            // Step 1: Copy elements into a vector
                        vector<pair<string, string>> vec(Grammar.begin(), Grammar.end());

                        // // Step 2: Reverse the vector
                        // reverse(vec.begin(), vec.end());

                        // Step 3: Insert into a new unordered_multimap
                        unordered_multimap<string, string> reversed_Grammar;
                        for (const auto &pair : vec) {
                            reversed_Grammar.insert(pair);
                        }
                        Grammar = reversed_Grammar;
        for (const auto &pair : Grammar) {
            string non_terminal = pair.first;
            set<char> follow_set = CalculateFollow(non_terminal);
            FollowSets[non_terminal] = follow_set;
        }
        // Debug output
        cout << "Follow Sets: " << endl;
        for (const auto &pair : FollowSets) {
            cout << pair.first << " -> ";
            for (char ch : pair.second) {
                cout << ch << " ";
            }
            cout << endl;
        }
    }
    // set<char> CalculateFollow(const string &non_terminal) {
    //     set<char> follow_set;
    //     if (non_terminal == "<Exp>") {
    //         follow_set.insert('$');
    //     }
    //     for (const auto &pair : Grammar) {
    //         string production = pair.second;
    //         for (int i = 0; i < production.size() - 1; i++) {
    //             if (production[i] =='<')
    //             {
    //                 string nonTerminal = "<";
    //                  i+=1;
    //                 while (i < production.size() && production[i]!= '>') {
    //                     nonTerminal += production[i];
    //                     i++;
    //                 }
    //                 nonTerminal += '>';
    //                 if (nonTerminal == non_terminal) {
    //                     char next_char = production[i + 1];
    //                     if (next_char != '<') {
    //                         follow_set.insert(next_char);
    //                     } 
    //                     else {
    //                             string nonTerminal2 = "<";
    //                             i+=1;
    //                             while (i < production.size() && production[i]!= '>') {
    //                                 nonTerminal2 += production[i];
    //                                 i++;
    //                             }
    //                             nonTerminal2 += '>';
    //                             set<char> nonTerminal2_first_set = FirstSets[nonTerminal2];
    //                             if (nonTerminal2_first_set.find('e') != nonTerminal2_first_set.end()) {
    //                                 // If the first set contains epsilon
    //                                 set<char> nonTerminal2_follow_set = CalculateFollow(nonTerminal2);
    //                                 follow_set.insert(nonTerminal2_follow_set.begin(), nonTerminal2_follow_set.end());
    //                             } 
    //                             else
    //                             {
    //                                 follow_set.insert(nonTerminal2_first_set.begin(), nonTerminal2_first_set.end());
    //                             }
                                
                               
    //                     }
    //                 } else
    //                 {
    //                    continue;
    //                 }
                    
    //             }
                
    //         } 
    //     }
    //     return follow_set;
    // }
    
    set<char> CalculateFollow(const string &non_terminal) {
        // Check if already computed
        if (FollowSets.find(non_terminal) != FollowSets.end()) {
            return FollowSets[non_terminal];
        }
    
        set<char> follow_set;
        
        // Start symbol gets '$' in its FOLLOW set
        if (non_terminal == "<Exp>") {
            follow_set.insert('$');
        }
    
        // Search through all productions
        for (const auto &[lhs, production] : Grammar) {
            for (size_t i = 0; i < production.size(); ) {
                if (production[i] == '<') {
                    // Extract non-terminal
                    string current_nt = "<";
                    i++;
                    while (i < production.size() && production[i] != '>') {
                        current_nt += production[i];
                        i++;
                    }
                    current_nt += '>';
    
                    // If this is our target non-terminal
                    if (current_nt == non_terminal) {
                        // Case 1: A → αBβ - add FIRST(β) (except ε) to FOLLOW(B)
                        if (i + 1 < production.size()) {
                            char next_char = production[i + 1];
                            
                            if (next_char != '<') {
                                // Next symbol is terminal
                                if (next_char != 'e') { // don't add epsilon
                                    follow_set.insert(next_char);
                                }
                            } else {
                                // Next symbol is non-terminal
                                string next_nt = "<";
                                i += 2;
                                while (i < production.size() && production[i] != '>') {
                                    next_nt += production[i];
                                    i++;
                                }
                                next_nt += '>';
    
                                // Add FIRST(next_nt) except ε
                                for (char c : FirstSets[next_nt]) {
                                    if (c != 'e') {
                                        follow_set.insert(c);
                                    }
                                }
    
                                // If ε ∈ FIRST(next_nt), add FOLLOW(lhs)
                                if (FirstSets[next_nt].count('e')) {
                                    set<char> lhs_follow = CalculateFollow(lhs);
                                    follow_set.insert(lhs_follow.begin(), lhs_follow.end());
                                }
                            }
                        } 
                        // Case 2: A → αB or A → αBβ where β ⇒* ε - add FOLLOW(A) to FOLLOW(B)
                        else {
                            if (lhs != non_terminal) { // avoid infinite recursion
                                set<char> lhs_follow = CalculateFollow(lhs);
                                follow_set.insert(lhs_follow.begin(), lhs_follow.end());
                            }
                        }
                    }
                    i++; // move past '>'
                } else {
                    i++; // skip terminals
                }
            }
        }
    
        FollowSets[non_terminal] = follow_set;
        return follow_set;
    }
// ==== Construct Parsing table using the grammar and first/follow sets
void ConstructParsingTable() {
    parsingTable.clear();  // Clear existing entries

    for (const auto& rule : Grammar) {
        const string& nonTerminal = rule.first;
        const string& production = rule.second;

        // Calculate FIRST set of the production string 
        set<char> firstOfProduction = CalculateFirstOfString(production);
        // Check if the production cpntains epsilon (e)
        if (firstOfProduction.count('e')){
        set<char> followSet = CalculateFollow(nonTerminal);
            for (char terminal : followSet) {
                auto key = make_pair(nonTerminal, terminal);
                // Check for conflicts
                if (parsingTable.find(key) != parsingTable.end()) {
                    cerr << "Error: Grammar is not LL(1) - conflict for ("
                         << nonTerminal << ", " << terminal << ")\n";
                    cerr << "Existing: " << parsingTable[key] 
                         << ", New: " << production << endl;
                    return;
                }
                parsingTable[key] = production;
            }
        }
          // Case 2: For each terminal in FIRST(production)
       else{
        for (char terminal : firstOfProduction) {
           
                auto key = make_pair(nonTerminal, terminal);
                // Check for conflicts (grammar isn't LL(1) if conflict exists)
                if (parsingTable.find(key) != parsingTable.end()) {
                    cerr << "Error: Grammar is not LL(1) - conflict for ("
                         << nonTerminal << ", " << terminal << ")\n";
                    cerr << "Existing: " << parsingTable[key] 
                         << ", New: " << production << endl;
                    return;
                }
                parsingTable[key] = production;
          
        }
    }

    }
}

// Helper function to calculate FIRST of a string (sequence of symbols)
set<char> CalculateFirstOfString(const string& str) {
    set<char> result;
    bool containsEpsilon = true;

    for (size_t i = 0; i < str.size() && containsEpsilon; ) {
        if (str[i] == '<') {
            // Extract non-terminal
            string nt;
            nt += str[i++];  // Add '<'
            while (i < str.size() && str[i] != '>') {
                nt += str[i++];
            }
            if (i < str.size()) nt += str[i++];  // Add '>'

            // Add FIRST(nt) to result (excluding ε)
            for (char c : FirstSets[nt]) {
                if (c != 'e') result.insert(c);
            }

            // Check if ε is in FIRST(nt)
            if (FirstSets[nt].count('e') == 0) {
                containsEpsilon = false;
            }
        } else {
            // Terminal symbol
            result.insert(str[i]);
            containsEpsilon = false;
            i++;
        }
    }

    // If all symbols can derive ε, add ε to the result
    if (containsEpsilon) {
        result.insert('e');
    }

    return result;
}
   // ==== Parse the input tokens using Stack ====
void StartParsing() {
    vector<string> stack;
    stack.push_back("$");
    stack.push_back("<Exp>");
    
    int index = 0;
    string current_token = (index < Tokens.size()) ? Tokens[index] : "$";
    
    while (!stack.empty()) {
        string top = stack.back();
        stack.pop_back();
        
        // Debug output
        output_file << "Stack top: " << top << ", Current token: " << current_token << endl;
        
        if (top == current_token) {
            // Terminal match - advance to next token
            index++;
            current_token = (index < Tokens.size()) ? Tokens[index] : "$";
        }
        else if (Grammar.find(top) != Grammar.end()) {
            // Non-terminal - use parsing table
            pair<string, char> key = make_pair(top, current_token[0]);
            
            if (parsingTable.find(key) != parsingTable.end()) {
                string production = parsingTable[key];
                
                // Handle epsilon production
                if (production == "e") {
                    output_file << "Applying production: " << top << " -> " << production << endl;
                    continue;
                }
                
                output_file << "Applying production: " << top << " -> " << production << endl;
                
                // Push production symbols in reverse order
                string symbol;
                for (int i = production.length() - 1; i >= 0; ) {
                    if (production[i] == '>') {
                        // Extract non-terminal
                        int j = i;
                        while (j >= 0 && production[j] != '<') j--;
                        if (j >= 0) {
                            symbol = production.substr(j, i - j + 1);
                            stack.push_back(symbol);
                            i = j - 1;
                        }
                    } else {
                        // Terminal
                        symbol = string(1, production[i]);
                        stack.push_back(symbol);
                        i--;
                    }
                }
            } else {
                output_file << "Error: No production found for (" << top 
                           << ", " << current_token << ")" << endl;
                return;
            }
        } else {
            output_file << "Error: Unexpected token " << current_token 
                       << ", expected " << top << endl;
            return;
        }
    }
    
    if (current_token != "$") {
        output_file << "Error: Input not fully consumed" << endl;
        return;
    }
    
    output_file << "Parsing completed successfully!" << endl;
}
};

#endif // PARSER_H