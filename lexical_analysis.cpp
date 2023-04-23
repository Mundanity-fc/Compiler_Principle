/* Created by Mundanity on 23-4-8.
 * Encoding with UTF-8
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

Analyser::~Analyser() = default;

void Analyser::generate() {
    // 加载产生式文件
    std::ifstream generate;
    generate.open(this->grammar_file);
    if(!generate){
        std::cout << "No such File" << std::endl;
    }

    // 按行读取产生式文件
    std::string line;
    bool is_get_starter = false;
    while (getline(generate, line)){
        // 忽略注释
        if(line[0] == '#')
            continue;
        // 忽略空行
        if(line.empty())
            continue;
        std::vector<std::string> production = this->divide_line(line);
        if(!is_get_starter){

        }
        std::cout << line << std::endl;
    }
    std::cout << std::endl;
}

std::vector<std::string> Analyser::divide_line(std::string production) {
    std::vector<std::string> divided;
    int max_length = production.length();
    int divide = production.find("->");
    std::string leftPart = production.substr(0, divide - 1);
    std::string rightPart = production.substr(divide + 2, max_length);
    divided.push_back(leftPart);
    divided.push_back(rightPart);
    return divided;
}

void Analyser::clean_space(std::string &str) {
    auto location = str.find(' ');
    while (location != std::string::npos){
        str.erase(location,1);
        location = str.find(' ');
    }
}
