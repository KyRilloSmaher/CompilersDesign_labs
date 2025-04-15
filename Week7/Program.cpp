#include<iostream>
#include<string>
#include "Parser.h"

using namespace std;

int main(int argc, char **arg){

    string INPUT_filename ="input.txt";
    string OUTPUT_filename ="ScannerOutput.txt";
    string Parser_Output_filename ="ParserOutput.txt";
    LL1Parser parser = LL1Parser(INPUT_filename,OUTPUT_filename,Parser_Output_filename);
    


    return 0;
    
}