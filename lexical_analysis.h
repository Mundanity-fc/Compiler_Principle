/* Created by Mundanity on 23-4-8.
 * Encoding with UTF-8
 * 词法分析程序，采用 DFA 实现词法分析，即给定输入可以确定下一状态位置
 *
 * token 可由关键词，标识符，常量，限定符和运算符组成
 * */

#include <vector>
#include <string>

struct Node{
    std::string name;
};

struct Edge{
    Node* prevNode;
    Node* nextNode;
    std::string target;
};

// 文法G的四元组
struct Grammar{
    // VN, 非终结符集
    std::vector<std::string> VN;
    // VT, 终结符集
    std::vector<std::string> VT;
    // P, 产生式集合
    std::vector<std::string> P;
    // S, 开始符合
    std::string S;
};

class Analyser{
private:
    // 文法产生式位置
    std::string grammar_file;
    // 文法 G 的参数
    Grammar G;

public:
    // 构造函数
    Analyser(std::string filename);
    // 析构函数
    ~Analyser();
    //
    void generate();
    // 产生式分割，将产生式划分为左部与右部
    std::vector<std::string> divide_line(std::string production);
    //清除多余空格
    void clean_space(std::string &str);
};

