#include <iostream>
#include "lexical_analysis.h"

int main() {
    std::cout << "词法分析" << std::endl;
    Lexical_Analysis Task1("001_input_lexical_input_grammar.txt");
    Task1.generate();
    Task1.print_NFA();
    Task1.check("001_input_sourcecode.txt");
    Task1.print_result();
    Task1.save_result();
}
