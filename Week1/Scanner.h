#ifndef SCANNER
#define SCANNER

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

class Scanner {
private:
    ifstream INPUT_FILE;
    ofstream OUTPUT_FILE;
    set<string> AvailableColors = {"red", "green", "blue", "yellow", "white"};
    set<string> nodes;
    set<string> colors;

public:
    Scanner(const string& INPUT_FILEName, const string& OUTPUT_FILEName) {
        INPUT_FILE.open(INPUT_FILEName);
        if (!INPUT_FILE.is_open()) {
            cerr << "Error: Unable to open input file: " << INPUT_FILEName << endl;
            throw runtime_error("File not found");
        }

        OUTPUT_FILE.open(OUTPUT_FILEName);
        if (!OUTPUT_FILE.is_open()) {
            cerr << "Error: Unable to open output file: " << OUTPUT_FILEName << endl;
            throw runtime_error("File not found");
        }
    }

    void scan() {
        char chr;
        string str;

        while (INPUT_FILE.get(chr)) {
            while (isspace(chr) && INPUT_FILE.peek() != EOF) { 
                INPUT_FILE.get(chr);
            }

            if (chr == '(') {
                OUTPUT_FILE << "<LEFTPARENTHESIS>\n";
            } else if (chr == ')') {
                OUTPUT_FILE << "<RIGHTPARENTHESIS>\n";
            } else if (chr == ',') {
                OUTPUT_FILE << "<COMMA>\n";
            } else if (isdigit(chr)) { 
                str = "";
                str += chr;

                while (INPUT_FILE.get(chr) && isdigit(chr)) {
                    str += chr;
                }

                if (!isdigit(chr) && !INPUT_FILE.eof()) {
                    INPUT_FILE.putback(chr);
                }

                nodes.insert(str);
                OUTPUT_FILE << "<NUMBER_" << str << ">\n";
            } else if (isalpha(chr)) { 
                str = "";
                str += chr;

                while (INPUT_FILE.get(chr) && isalpha(chr)) {
                    str += chr;
                }

                if (!isalpha(chr) && !INPUT_FILE.eof()) {
                    INPUT_FILE.putback(chr);
                }

                if (AvailableColors.find(str) != AvailableColors.end()) {
                    colors.insert(str);
                    OUTPUT_FILE << "<COLOR_" << str << ">\n";
                    
                  
                    if (isdigit(INPUT_FILE.peek())) {
                        string errorStr = str;
                        while (isdigit(INPUT_FILE.peek())) {
                            char numChar;
                            INPUT_FILE.get(numChar);
                            errorStr += numChar;
                        }
                        OUTPUT_FILE << "<ERROR>\n";
                        return;
                    }
                } else {
                    OUTPUT_FILE << "<ERROR>\n";
                    cerr << "Invalid color: " << str << endl;
                    return;
                }
            }
        }
    }

    void print_nodes() const {
        cout << "\nNodes USED IN STRING:\n";
        for (const auto& i : nodes) {
            cout << i << "\n";
        }
    }

    void print_colors() const {
        cout << "\nColors USED IN STRING: :\n";
        for (const auto& i : colors) {
            cout << i << "\n";
        }
    }

    ~Scanner() {
        if (INPUT_FILE.is_open()) INPUT_FILE.close();
        if (OUTPUT_FILE.is_open()) OUTPUT_FILE.close();
    }
};



#endif