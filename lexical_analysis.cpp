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
Lexical_Analysis::Lexical_Analysis(std::string filename){
    this->grammar_file = std::move(filename);
}

// 析构函数
Lexical_Analysis::~Lexical_Analysis() = default;

// NFA 生成
void Lexical_Analysis::generate() {
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
        std::vector<std::string> production = Lexical_Analysis::divide_line(line);

        if(!this->is_in_VN(production[0])){
            this->G.VN.push_back(production[0]);
        }

        // 添加单字母节点到初态集合
        std::string current_family;
        if(production[0].size() == 1)
            this->G.S.push_back(production[0]);
        // 将本行产生式加入到 G 中
        this->G.P.push_back(line);

        // 分割右部，开始建立图
        std::vector<std::string> rights = Lexical_Analysis::split_right(production[1]);
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
                    if(!this->is_in_VT(right))
                        this->G.VT.push_back(right);
                }else{
                    Record rec;
                    rec.start = production[0];
                    rec.end = right.substr(set_stop+1, right.size()-set_stop);
                    rec.edge = right.substr(0, set_stop+1);
                    this->Net.push_back(rec);
                    if(!this->is_in_VT(right.substr(0, set_stop+1)))
                        this->G.VT.push_back(right.substr(0, set_stop+1));
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
                if(!this->is_in_VT(right))
                    this->G.VT.push_back(right);
                continue;
            }
            // 一位的终止符和一个非终止符
            Record rec;
            rec.start = production[0];
            rec.end = right.substr(1,right.size()-1);
            rec.edge = right.substr(0,1);
            this->Net.push_back(rec);
            if(!this->is_in_VT(right.substr(0,1)))
                this->G.VT.push_back(right.substr(0,1));
        }
    }

    //
    for(int i = 0; i < this->Net.size(); i++)
    {
        if (this->Net[i].start.size() == 1)
            this->Map.InitialNode.push_back(Net[i]);
        else
            this->Map.ProcessNode.push_back(Net[i]);
    }
}

// 打印 NFA
void Lexical_Analysis::print_NFA() {
    for (auto & i : this->Net) {
        std::cout << i.start << "--" << i.edge << "->" << i.end << std::endl;
    }
}

// token 检查
void Lexical_Analysis::check(std::string filename) {
    // 加载产生式文件
    std::ifstream sourcecode;
    sourcecode.open(filename);
    if(!sourcecode){
        std::cout << "No such File" << std::endl;
    }
    std::string line;
    int line_number = 0;
    std::string start;
    std::string current_node;
    std::string type;
    bool is_searched = true;
    while (getline(sourcecode, line)){
        line_number++;
        int index = 0;
        int max_length = line.size();
        while (index < max_length){
            // 忽略空格
            if (line[index] == ' '){
                index++;
                continue;
            }

            // 判断输入字符类型
            std::string compare;
            if (std::isalpha(line[index]))
                compare = "<alphabet>";
            else if (std::isdigit(line[index]))
                compare = "<number>";
            else
                compare = line.substr(index, 1);

            // 寻找初态
            if(current_node.empty()){
                for (int i = 0; i < this->Map.InitialNode.size(); ++i) {
                    if (this->Map.InitialNode[i].edge == compare){
                        start = Map.InitialNode[i].start;
                        current_node = Map.InitialNode[i].end;
                        is_searched = false;
                        break;
                    }
                }
            }

            // 进行图搜索
            while (index < max_length && (!is_searched)){
                index++;
                // 确认新比较符号
                if (std::isalpha(line[index]))
                    compare = "<alphabet>";
                else if (std::isdigit(line[index]))
                    compare = "<number>";
                else if (line[index] == ' ' || line[index] == 0)
                    compare = "ε";
                else
                    compare = line.substr(index, 1);
                // 以新的比较符号
                current_node = this->search_next_node(compare, current_node, this->Map.ProcessNode);
                if(current_node == "none"){
                    type = "错误";
                    current_node.clear();
                    is_searched = true;
                }
            }

            index++;
        }
        std::cout<<line<<std::endl;
    }
}

// 产生式分割，将产生式划分为左部与右部
std::vector<std::string> Lexical_Analysis::divide_line(const std::string& production) {
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
std::vector<std::string> Lexical_Analysis::split_right(std::string right) {
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
void Lexical_Analysis::clean_space(std::string &str) {
    auto location = str.find(' ');
    while (location != std::string::npos){
        str.erase(location,1);
        location = str.find(' ');
    }
}

bool Lexical_Analysis::is_in_VT(std::string target){
    for(const auto & i : this->G.VT){
        if (i == target)
            return true;
    }
    return false;
}

bool Lexical_Analysis::is_in_VN(std::string target){
    for(const auto & i : this->G.VN){
        if (i == target)
            return true;
    }
    return false;
}

std::string Lexical_Analysis::search_next_node(std::string target, std::string current_node, std::vector<Record> list) {
    std::string result = "none";
    for (int i = 0; i < list.size(); ++i) {
        if (list[i].start == current_node)
            if (list[i].edge == target)
                result = list[i].end;
    }
    return result;
}
