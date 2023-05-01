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
#include <utility>

// 构造函数
Analyser::Analyser(std::string filename){
    this->grammar_file = std::move(filename);
}

// 析构函数
Analyser::~Analyser() = default;

// NFA 生成
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

        // 将产生式划为左右两部分
        std::vector<std::string> production = Analyser::divide_line(line);

        // 添加单字母节点到初态集合
        std::string current_family;
        if(production[0].size() == 1)
            this->G.S.push_back(production[0]);
        // 将本行产生式加入到 G 中
        this->G.P.push_back(line);

        // 分割右部，开始建立图
        std::vector<std::string> rights = Analyser::split_right(production[1]);
        // 以分割的右部建立图
        for (auto & right : rights) {
            // 带集合的右部
            if (right.find('<') != std::string::npos && right.find('>') != std::string::npos){
                // 找到集合终止位置
                auto set_stop = right.find('>');
                if (set_stop == right.size()-1){
                    // 右部只含有集合
                    Record rec;
                    rec.start = production[0];
                    rec.end = "terminate";
                    rec.edge = right;
                    this->Net.push_back(rec);
                }else{
                    Record rec;
                    rec.start = production[0];
                    rec.end = right.substr(set_stop+1, right.size()-set_stop);
                    rec.edge = right.substr(0, set_stop+1);
                    this->Net.push_back(rec);
                }
                continue;
            }
            // 单终止符的右部
            if (right.size() == 1 || right == "ε"){
                Record rec;
                rec.start = production[0];
                rec.end = "terminate";
                rec.edge = right;
                this->Net.push_back(rec);
                continue;
            }
            // 一位的终止符和一个非终止符
            Record rec;
            rec.start = production[0];
            rec.end = right.substr(1,right.size()-1);
            rec.edge = right[0];
            this->Net.push_back(rec);
        }
    }
    std::cout << std::endl;
}

// DFA 生成
void Analyser::DFA() {

}

// token 检查
void Analyser::check(std::string line) {

}

// 产生式分割，将产生式划分为左部与右部
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

// 产生式右侧以'|'再次进行分割
std::vector<std::string> Analyser::split_right(std::string right) {
    std::string current_right = std::move(right);
    std::vector<std::string> result;
    auto location = current_right.find('|');
    while (location != std::string::npos){
        result.push_back(current_right.substr(0,location));
        current_right = current_right.substr(location+1);
        location = current_right.find('|');
    }
    // 分割剩下的最后一部分
    result.push_back(current_right);
    return result;
}

//清除多余空格
void Analyser::clean_space(std::string &str) {
    auto location = str.find(' ');
    while (location != std::string::npos){
        str.erase(location,1);
        location = str.find(' ');
    }
}