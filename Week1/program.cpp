#include <iostream>
#include <string>
#include "Scanner.h"
using namespace std;

int main() {
    
    Scanner scanner("INPUT.txt", "OUTPUT.txt");
    scanner.scan();

    scanner.print_nodes();
    scanner.print_colors();

    return 0;
}