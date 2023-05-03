//
// Created by Munda on 2023/5/2.
//
#include <iostream>
#include <vector>

struct Validation{
    std::string name;
    std::string result;
};

class Grammar_Analysis {
private:
    // VT 集合
    std::vector<std::string> VT;

    // VN 集合
    std::vector<std::string> VN;

    // 结果集合
    std::vector<Validation> Results;

    // token 列表
    std::vector<std::string> token_list;

    // 产生式列表
    std::vector<std::vector<std::string>> productions;

    // first 集
    std::vector<std::vector<std::vector<std::string>>> first_set;

    // follow 集
    std::vector<std::vector<std::vector<std::string>>> follow_set;

    // select 集
    std::vector<std::vector<std::vector<std::string>>> select_set;

    // 拆分 Task1 输出的 token 表
    std::string split_token(std::string line);

    // 将一行产生式分为两部分
    std::vector<std::string> divide_line(const std::string &production);

    // 分割右部
    std::vector<std::string> split_right(std::string right);

    // 清除空格
    void clean_space(std::string &str);

    // 内容是否在表中
    bool is_contain(std::string content, std::vector<std::string> list);

    // 建立 first 集
    void get_first_sets();

    // 获取指定 first 集
    std::vector<std::vector<std::string>> get_first(const std::string& target);

    // 合并集合
    static std::vector<std::string> merge_set(std::vector<std::string> A, std::vector<std::string> B);

    // 建立 follow 集
    void get_follow_sets();

    // 建立 select 集
    void get_select_sets();

    // 获取指定 follow 集
    std::vector<std::vector<std::string>> get_follow(const std::string &target);

    // 从集合中移除
    std::vector<std::string> remove_from_set(std::string target, std::vector<std::string> set);

    // 集合间是否有公共元素
    bool is_has_common(std::vector<std::string> A, std::vector<std::string> B);

public:
    // 构造函数
    Grammar_Analysis();

    // 析构函数
    ~Grammar_Analysis();

    // 判断 LL(1) 类型
    bool check_is_LL1();

    // 进行 token 检查
    void do_check();

    // 打印结果
    void print_result();

    // 保存结果
    void save_result();
};

