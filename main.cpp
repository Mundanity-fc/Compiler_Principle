#include <iostream>
#include "lexical_analysis.h"

int main() {
    Lexical_Analysis Task1("001_input_lexical_input_grammar.txt");
    Task1.generate();
    Task1.print_NFA();
    Task1.check("001_input_sourcecode.txt");
    Task1.print_result();
}
