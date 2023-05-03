#include <iostream>
#include "lexical_analysis.h"
#include "grammar_analysis.h"

int main() {
    std::cout << "词法分析：" << std::endl;
    Lexical_Analysis Task1("001_input_lexical_input_grammar.txt");
    Task1.generate();
    Task1.print_NFA();
    Task1.check("001_input_sourcecode.txt");
    Task1.print_result();
    Task1.save_result();

    std::cout << "语法分析：" << std::endl;
    Grammar_Analysis Task2;
    if(Task2.check_is_LL1())
        std::cout << "符合LL(1)文法" << std::endl;
    else
        exit(1);
    Task2.do_check();
    Task2.print_result();
    Task2.save_result();
}
