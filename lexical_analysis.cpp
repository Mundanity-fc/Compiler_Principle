/* Created by Mundanity on 23-4-8.
 *
 * 词法分析程序，采用 DFA 实现词法分析，即给定输入可以确定下一状态位置
 *
 * token 可由关键词，标识符，常量，限定符和运算符组成
 * */

#include "lexical_analysis.h"
#include <string>
#include <fstream>
#include <iostream>

Analyser::Analyser(std::string filename){
    this->grammar_file = filename;
}

Analyser::~Analyser() {}

void Analyser::generate() {
    // 加载产生式文件
    std::ifstream generate;
    generate.open(this->grammar_file);
    if(!generate){
        std::cout << "No such File" << std::endl;
    }

    // 按行读取产生式文件
    std::string line;
    while (getline(generate, line)){
        // 忽略注释
        if(line[0] == '#')
            continue;
        // 忽略空行
        if(line.empty())
            continue;
        std::cout << line << std::endl;
    }
    std::cout << std::endl;
}
