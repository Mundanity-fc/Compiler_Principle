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

            // 寻找初始态
            if(current_node.empty()){
                for (int i = 0; i < this->Map.InitialNode.size(); ++i) {
                    if (this->Map.InitialNode[i].edge == compare){
                        start = Map.InitialNode[i].start;
                        current_node = Map.InitialNode[i].end;
                        break;
                    }
                }
            }

            // 进行判断
            this->start_search(line[index], current_node, index, max_length, line, line_number, start);
        }
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

// 检查元素是否在 VT 中
bool Lexical_Analysis::is_in_VT(std::string target){
    for(const auto & i : this->G.VT){
        if (i == target)
            return true;
    }
    return false;
}

// 检查元素是否在 VN 中
bool Lexical_Analysis::is_in_VN(std::string target){
    for(const auto & i : this->G.VN){
        if (i == target)
            return true;
    }
    return false;
}

// 依照 NFA 的图进行检查
void Lexical_Analysis::start_search(char initial, std::string &current_node, int &index, int max_length, std::string line, int line_number, std::string start) {
    // 检查终止 flag
    bool is_queue_built = false;
    // token 错误 flag
    bool is_failed = false;
    // 先将第一个字符加入到结果串中
    std::string finish;
    finish += initial;
    // 获取目前节点所有可用的边
    std::vector<std::string> available_token;
    available_token = this->get_available_token(current_node);
    // 当只有一个空串边时，直接结束判断。
    if (available_token.size() == 1)
        if (available_token[0] == "terminate")
            is_queue_built = true;
    // 当目前为终止节点时，直接结束判断
    if (current_node == "terminate")
        is_queue_built = true;

    // 开始循环检查
    while (index < max_length-1 && !is_queue_built){
        // 判断下一位输入的类型
        std::string next = line.substr(index+1, 1);
        if (std::isdigit(line[index+1]))
            next = "<number>";
        else if (std::isalpha(line[index+1]))
            // 对于常量检查时，字母需要额外定义
            if (start == "C" && (line[index+1] == 'e' || line[index+1] == 'E' || line[index+1] == 'i'))
                next = line.substr(index+1, 1);
            else
                next = "<alphabet>";
        else
            next = line.substr(index+1, 1);

        // 检查下一位是否在可用边列表里
        if(is_contain(next, available_token)){
            // 下一位可用，塞入结果串中
            finish += line[index+1];
            index++;
            // 更新目前节点
            for (int i = 0; i < this->Map.ProcessNode.size(); ++i) {
                if (this->Map.ProcessNode[i].edge == next && this->Map.ProcessNode[i].start == current_node){
                    current_node = Map.ProcessNode[i].end;
                    break;
                }
            }

            // 新节点是结束节点，则直接结束
            if (current_node == "terminate")
                is_queue_built = true;

            // 更新可用边列表
            available_token = this->get_available_token(current_node);
        } else{
            // 下一位不可用，则结束搜索
            is_queue_built = true;
            // 当下一位不可用而且目前节点无空串边，则说明目前的匹配失败
            if (!this->is_contain("ε", available_token))
                is_failed = true;
        }
    }
    // 行检测标记右移一位
    index++;
    // 清楚目前节点（应该可删，因为在上面有更新）
    current_node.clear();
    // 新建一条结果记录
    Result new_result;
    new_result.line_number = line_number;
    new_result.token = finish;
    if (is_failed)
        new_result.type = "错误类型";
    else{
        if(start =="I") {
            if(is_reserved(finish))
            {
                new_result.type = "关键字";
            }else
                new_result.type = "标识符";
        }
        if(start =="D")
            new_result.type = "界符";
        if(start =="C")
            new_result.type = "常量";
        if(start =="O")
            new_result.type = "操作符";
    }
    // 结果塞入到结果列表中
    this->Result_list.push_back(new_result);
}

// 检查标识符是否为表留关键字
bool Lexical_Analysis::is_reserved(std::string target) {
    for (const auto & i : this->Reserved_Identifier) {
        if (i == target)
            return true;
    }
    return false;
}

// 获取所有可用的边
std::vector<std::string> Lexical_Analysis::get_available_token(std::string current_node) {
    std::vector<std::string> list;
    for (int i = 0; i < this->Map.ProcessNode.size(); ++i) {
        if (this->Map.ProcessNode[i].start == current_node)
            list.push_back(this->Map.ProcessNode[i].edge);
    }
    return list;
}

// 检查 content 内容是否在 list 当中
bool Lexical_Analysis::is_contain(std::string content, std::vector<std::string> list) {
    for (const auto & i : list) {
        if (i == content)
            return true;
    }
    return false;
}

// 打印结果
void Lexical_Analysis::print_result() {
    for (auto & i : this->Result_list) {
        std::cout << i.line_number << " " << i.type << " " << i.token << std::endl;
    }
}

// 保存结果到文件
void Lexical_Analysis::save_result() {

}




