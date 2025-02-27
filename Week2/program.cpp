#include<iostream>
#include<string>
#include"Scanner.h"
#include"DFA.h"
using namespace std;

int main(int argc, char **arg){

    string INPUT_filename ="input.txt";
    string OUTPUT_filename ="output.txt";
    Scanner scanner = Scanner(INPUT_filename,OUTPUT_filename);
    scanner.scan();

    DFA dfa = DFA();
    string STRING ="bbbbbabbab";
    if (dfa.check(STRING))
    {
        cout << STRING << " is accepted by the DFA" << endl;
    }
    else
    {
        cout << STRING << " is rejected by the DFA" << endl;
    }
    


    return 0;
    
}