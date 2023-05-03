//
// Created by Munda on 2023/5/2.
//
#include <iostream>
#include <fstream>
#include "grammar_analysis.h"

// 构造函数
Grammar_Analysis::Grammar_Analysis() {
    // 读取 token 表
    std::ifstream tokens;
    tokens.open("001_output_token.txt");
    if(!tokens){
        std::cout << "No such File" << std::endl;
        exit(1);
    }
    std::string line;
    while (getline(tokens, line)) {
        this->token_list.push_back(this->split_token(line));
    }
    tokens.close();
    // 读取语法表
    std::ifstream grammar;
    grammar.open("002_input_grammar.txt");
    if(!grammar){
        std::cout << "No such File" << std::endl;
        exit(1);
    }
    std::string grammar_line;
    while (getline(grammar, grammar_line)) {
        // 忽略空格
        if (grammar_line[0] == '#'){
            continue;
        }
        std::vector<std::string> production = this->divide_line(grammar_line);
        if(!is_contain(production[0], this->VN))
            this->VN.push_back(production[0]);
        std::vector<std::string> rights = this->split_right(production[1]);
        for (const auto & right : rights) {
            std::vector<std::string> split_production;
            split_production.push_back(production[0]);
            split_production.push_back(right);
            this->productions.push_back(split_production);
            if(!is_contain(right.substr(0, 1), this->VT))
                this->VT.push_back(right.substr(0, 1));
        }
    }
    grammar.close();
}

// 析构函数
Grammar_Analysis::~Grammar_Analysis() = default;

// 分割 token 表
std::string Grammar_Analysis::split_token(std::string line) {
    std::string current_line = std::move(line);
    std::vector<std::string> result;
    auto location = current_line.find(' ');
    while (location != std::string::npos){
        result.push_back(current_line.substr(0,location));
        current_line = current_line.substr(location+1);
        location = current_line.find(' ');
    }
    // 分割剩下的最后一部分
    result.push_back(current_line);
    return result[2];
}

// 产生式分割，将产生式划分为左部与右部
std::vector<std::string> Grammar_Analysis::divide_line(const std::string& production) {
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

//清除多余空格
void Grammar_Analysis::clean_space(std::string &str) {
    auto location = str.find(' ');
    while (location != std::string::npos){
        str.erase(location,1);
        location = str.find(' ');
    }
}

// 检查 content 内容是否在 list 当中
bool Grammar_Analysis::is_contain(std::string content, std::vector<std::string> list) {
    for (const auto & i : list) {
        if (i == content)
            return true;
    }
    return false;
}

// 产生式右侧以'|'再次进行分割
std::vector<std::string> Grammar_Analysis::split_right(std::string right) {
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

// 检查是否为 LL(1) 文法
bool Grammar_Analysis::check_is_LL1() {
    std::string a = this->productions[8][1].substr(0, 1);
    if (a == "B"){
        std::cout;
    }
    this->get_first_sets();
    //this->get_follow_sets();
    this->get_select_sets();
    for (int i = 0; i < this->select_set.size(); ++i) {
        std::string name;
        std::vector<std::string> set;
        name = this->select_set[i][0][0];
        set = this->select_set[i][1];
        // 向下查询
        for (int j = i + 1; j < this->select_set.size(); ++j) {
            // 若左部相同
            if (this->select_set[j][0][0][0] == name[0]){
                // 若有相同元素
                if(this->is_has_common(set, this->select_set[j][1]))
                    return false;
            } else
                // 无相同左部直接跳过
                continue;
        }
    }
    return true;
}

// 建立 first 集
void Grammar_Analysis::get_first_sets() {
    bool is_changed = true;
    while (is_changed){
        is_changed = false;
        for (auto & production : this->productions) {
            std::vector<std::vector<std::string>> first;
            // 由 VT 开头的产生式的 first 集
            if (is_contain(production[1].substr(0, 1), this->VT)){
                first = this->get_first(production[0].substr(0, 1));
                // 没有已有的 first 集时
                if(first.size() == 0){
                    std::vector<std::string> name;
                    std::vector<std::string> element;
                    name.push_back(production[0]);
                    element.push_back(production[1].substr(0, 1));
                    first.push_back(name);
                    first.push_back(element);
                    is_changed = true;
                    this->first_set.push_back(first);
                }else{
                    // 存在现有的 first 集时
                    // 有不存在的元素
                    if (!is_contain(production[1].substr(0, 1), first[1]))
                    {
                        first[1].push_back(production[1].substr(0, 1));
                        // 替换已有的 first 集
                        for (int i = 0; i < this->first_set.size(); ++i) {
                            if (this->first_set[i][0] == first[0]) {
                                this->first_set[i] = first;
                                break;
                            }
                        }
                        is_changed = true;
                    } else
                        continue;
                }
                // 由 VN 开头时
            }else{
                std::vector<std::vector<std::string>> first;
                // 获取第一个 VN 的 first 集
                first = this->get_first(production[1].substr(0, 1));
                if (first.size() != 0){
                    // 存在集合，先修改集合所属
                    first[0][0] = production[0].substr(0, 1);
                    // 找到老集合，以及其元素个数
                    std::vector<std::vector<std::string>> old_first;
                    old_first = this->get_first(production[0].substr(0, 1));
                    first[1] = this->merge_set(old_first[1], first[1]);
                    if(old_first[1].size() != first[1].size()){
                        is_changed = true;
                        // 替换已有的 first 集
                        for (int i = 0; i < this->first_set.size(); ++i) {
                            if (this->first_set[i][0] == first[0]) {
                                this->first_set[i] = first;
                                break;
                            }
                        }
                    } else
                        continue;
                } else{
                    // 不存在集合，等待下一次创建
                    continue;
                }
            }
        }
    }
}

// 寻找指定的 first 集
std::vector<std::vector<std::string>> Grammar_Analysis::get_first(const std::string& target) {
    std::vector<std::vector<std::string>> result;
    for (int i = 0; i < this->first_set.size(); ++i) {
        if (this->first_set[i][0][0] == target)
            result = first_set[i];
    }
    return result;
}

// 合并两个集合
std::vector<std::string> Grammar_Analysis::merge_set(std::vector<std::string> A, std::vector<std::string> B) {
    if (A.size() == 0)
        return B;
    for (int i = 0; i < B.size(); ++i) {
        bool all_different = true;
        for (int j = 0; j < A.size(); ++j)
            if (B[i] == A[j])
                all_different = false;
        if (all_different)
            A.push_back(B[i]);
    }
    return A;
}

// 建立 follow 集
void Grammar_Analysis::get_follow_sets() {
    bool is_changed = true;
    while (is_changed){
        is_changed = false;
        for (int i = 0; i < this->VN.size(); ++i) {
            std::string current = VN[i];
            std::vector<std::vector<std::string>> follow = this->get_follow(current);
            // 若暂无 follow 集合
            if (follow[1].size() == 0){
                // 把 # 加入到开始符号里
                if (current == "A"){
                    is_changed = true;
                    std::vector<std::string> name;
                    name.push_back(current);
                    follow[0] = name;
                    std::vector<std::string> set;
                    set.push_back("#");
                    follow[1] = set;
                    this->follow_set.push_back(follow);
                    continue;
                }
                // 定义一个空的 follow 集合
                std::vector<std::string> name;
                name.push_back(current);
                std::vector<std::string> set;
                follow.push_back(name);
                follow.push_back(set);
                // 若存在 follow 集合
                // 遍历所有产生式
                for (int j = 0; j < this->productions.size(); ++j) {
                    // 遍历右部所有位置
                    for (int k = 0; k < this->productions[j][1].size(); ++k) {
                        // 找到了 current 的位置
                        std::string location = this->productions[j][1].substr(k, 1);
                        if (location == current){
                            // 右侧还有元素
                            if (k + 1 < this->productions[j][1].size()){
                                // 导入右侧的 first 集合
                                std::vector<std::string> first = this->get_first(this->productions[j][1].substr(k+1, 1))[1];
                                first = this->remove_from_set("*", first);
                                std::vector<std::string> new_follow;
                                new_follow = merge_set(first, follow[1]);
                                if (new_follow.size() != follow[1].size()){
                                    follow[1] = new_follow;
                                    is_changed = true;
                                }
                                // 导入右侧的 follow 集合
                                for (int l = 0; l < this->productions.size(); ++l) {
                                    // 直接推导出空串的 VN，并入其 follow 集合
                                    if (this->productions[l][0] == this->productions[j][1].substr(k+1, 1))
                                        if (this->productions[l][1] == "*"){
                                            std::vector<std::string> neighbor_follow = this->get_follow(this->productions[l][0])[1];
                                            neighbor_follow = merge_set(neighbor_follow, follow[1]);
                                            if(neighbor_follow.size() != follow[1].size()){
                                                follow[1] = neighbor_follow;
                                                is_changed = true;
                                            }
                                        }

                                }

                            } else{
                                // 右侧无元素，直接加入产生式左侧的 follow 集（相当于右侧能直接推导出空集）
                                std::vector<std::string> left_follow = this->get_follow(this->productions[j][0])[1];
                                left_follow = merge_set(left_follow, follow[1]);
                                if(left_follow.size() != follow[1].size()){
                                    follow[1] = left_follow;
                                    is_changed = true;
                                }
                            }
                        }
                    }
                }
                // 导入目前的 follow 集
                this->follow_set.push_back(follow);
            }else{
                // 若存在 follow 集合
                // 遍历所有产生式
                for (int j = 0; j < this->productions.size(); ++j) {
                    // 遍历右部所有位置
                    for (int k = 0; k < this->productions[j][1].size(); ++k) {
                        // 找到了 current 的位置
                        if (this->productions[j][1].substr(k, 1) == current){
                            // 右侧还有元素
                            if (k + 1 < this->productions[j][1].size()){
                                // 导入右侧的 first 集合
                                std::vector<std::string> first = this->get_first(this->productions[j][1].substr(k+1, 1))[1];
                                first = this->remove_from_set("*", first);
                                std::vector<std::string> new_follow;
                                new_follow = merge_set(first, follow[1]);
                                if (new_follow.size() != follow[1].size()){
                                    follow[1] = new_follow;
                                    is_changed = true;
                                }
                                // 导入左侧的 follow 集合
                                for (int l = 0; l < this->productions.size(); ++l) {
                                    // 直接推导出空串的 VN，并入其 follow 集合
                                    if (this->productions[l][0] == this->productions[j][1].substr(k+1, 1))
                                        if (this->productions[l][1] == "*"){
                                            std::vector<std::string> neighbor_follow = this->get_follow(this->productions[j][0])[1];
                                            neighbor_follow = merge_set(neighbor_follow, follow[1]);
                                            if(neighbor_follow.size() != follow[1].size()){
                                                follow[1] = neighbor_follow;
                                                is_changed = true;
                                            }
                                        }

                                }

                            } else{
                                // 右侧无元素，直接加入产生式左侧的 follow 集（相当于右侧能直接推导出空集）
                                std::vector<std::string> left_follow = this->get_follow(this->productions[j][0])[1];
                                left_follow = merge_set(left_follow, follow[1]);
                                if(left_follow.size() != follow[1].size()){
                                    follow[1] = left_follow;
                                    is_changed = true;
                                }
                            }
                        }
                    }
                }
                // 覆盖原有 follow 集
                for (int j = 0; j < this->follow_set.size(); ++j) {
                    if (this->follow_set[j][0][0] == current){
                        this->follow_set[j] = follow;
                    }
                }
            }
        }
    }
}

// 获取指定的 follow 集
std::vector<std::vector<std::string>> Grammar_Analysis::get_follow(const std::string& target) {
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> temp;
    result.push_back(temp);
    result.push_back(temp);
    for (int i = 0; i < this->follow_set.size(); ++i) {
        if (this->follow_set[i][0][0] == target)
            result = follow_set[i];
    }
    return result;
}

// 建立 select 集
void Grammar_Analysis::get_select_sets() {
    for (auto & production : this->productions) {
        std::vector<std::vector<std::string>> select;
        // 产生式右部由 VT 开始
        if (is_contain(production[1].substr(0, 1), this->VT)){
            std::vector<std::string> name;
            name.push_back(production[0]+"->"+production[1]);
            select.push_back(name);
            std::vector<std::string> set;
            set.push_back(production[1].substr(0, 1));
            select.push_back(set);
            this->select_set.push_back(select);
        }else{
            select = get_first(production[1].substr(0, 1));
            select[0][0] = (production[0]+"->"+production[1]);
            this->select_set.push_back(select);
        }
    }
}

// 从指定集合中删除指定元素
std::vector<std::string> Grammar_Analysis::remove_from_set(std::string target, std::vector<std::string> set) {
    std::vector<std::string> result;
    result = set;
    for (int i = 0; i < result.size(); ++i) {
        result.erase(result.begin() + i);
        break;
    }
    return result;
}

// 两个集合是否有共同元素
bool Grammar_Analysis::is_has_common(std::vector<std::string> A, std::vector<std::string> B) {
    for (int i = 0; i < A.size(); ++i) {
        for (int j = 0; j < B.size(); ++j) {
            if (A[i] == B[i])
                return true;
        }
    }
    return false;
}

// 进行 token 检查
void Grammar_Analysis::do_check() {
    for (int i = 0; i < token_list.size(); ++i) {
        std::string message;
        int invalid_bit = 0;
        for (int j = 0; j < token_list[i].size(); ++j) {
            // 位合法检验 flag
            bool bit_valid = false;
            // 更换比较元素
            char compare = this->token_list[i][j];
            std::string str;
            if (std::isalpha(compare))
                str = "a";
            if (std::isdigit(compare)){
                str = "n";
                if (j == 0){
                    invalid_bit = -1;
                    message = "第" + std::to_string(j) + "位错误";
                    break;
                }
            }
            for (int k = 0; k < this->VT.size(); ++k) {
                if (str == this->VT[k])
                    bit_valid = true;
            }
            if (!bit_valid)
            {
                message = "第" + std::to_string(j) + "位错误";
                invalid_bit--;
            }
        }
        Validation new_result;
        new_result.name = this->token_list[i];
        if (invalid_bit == 0)
            new_result.result = "YES";
        else
            new_result.result = "NO";
        new_result.message = message;
        this->Results.push_back(new_result);
    }
}

// 打印结果
void Grammar_Analysis::print_result() {
    for (int i = 0; i < this->Results.size(); ++i)
        std::cout << this->Results[i].name << " " << this->Results[i].result << std::endl;
}

// 保存结果到文件
void Grammar_Analysis::save_result() {
    std::ofstream output;
    output.open("002_output_validation.txt");
    for (auto & i : this->Results) {
        output << i.name << " " << i.result << std::endl;
    }
}
