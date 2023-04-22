#include <iostream>
#include "lexical_analysis.h"

int main() {
    Analyser NFA("lexical_input_grammar.txt");
    NFA.generate();
}
