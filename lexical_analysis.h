/* Created by Mundanity on 23-4-8.
 *
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

class Analyser{
private:
    // 文法产生式位置
    std::string grammar_file;


public:
    // 构造函数
    Analyser(std::string filename);
    // 析构函数
    ~Analyser();

    void generate();
};

