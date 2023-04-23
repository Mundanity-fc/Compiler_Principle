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
        // 将产生式划为左右两部分
        std::vector<std::string> production = this->divide_line(line);
        // 第一次需添加开始符
        if(!is_get_starter){
            this->G.S = production[0];
            is_get_starter = true;
        }
        // 将本行产生式加入到 G 中
        this->G.P.push_back(line);

        // 以本行产生式建立节点，若节点不在类的 nodes 表中，则将该节点信息加入该表
        Node node;
        if (!this->is_created_node(production[0])){
            node.name = production[0];
            this->nodes.push_back(node);
        }
        Node* current_node = find_node(production[0]);
        std::vector<std::string> rights = this->split_right(production[1]);
        std::cout << line << std::endl;
    }
    std::cout << std::endl;
}

std::vector<std::string> Analyser::divide_line(const std::string& production) {
    std::vector<std::string> divided;
    int max_length = production.length();
    int divide = production.find("->");
    std::string leftPart = production.substr(0, divide - 1);
    this->clean_space(leftPart);
    std::string rightPart = production.substr(divide + 2, max_length);
    this->clean_space(rightPart);
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

bool Analyser::is_created_node(const std::string& name){
    for (int i = 0; i < this->nodes.size(); ++i) {
        if (this->nodes[i].name == name){
            return true;
        }
    }
    return false;
}

Node* Analyser::find_node(const std::string& name) {
    for (int i = 0; i < this->nodes.max_size(); ++i) {
        if (this->nodes[i].name == name){
            return &this->nodes[i];
        }
    }
    return nullptr;
}

std::vector<std::string> Analyser::split_right(std::string right) {
    std::string current_right = right;
    std::vector<std::string> result;
    auto location = current_right.find('|');
    while (location != std::string::npos){
        result.push_back(current_right.substr(0,location));
        current_right = current_right.substr(location+1);
        location = current_right.find('|');
    }
    return result;
}
