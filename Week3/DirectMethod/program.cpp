#include<iostream>
#include<string>
#include"Scanner.h"

using namespace std;

int main(int argc, char **arg){

    string INPUT_filename ="input.txt";
    string OUTPUT_filename ="tokens.txt";
    Scanner scanner = Scanner(INPUT_filename,OUTPUT_filename);
    scanner.scan();
    


    return 0;
    
}