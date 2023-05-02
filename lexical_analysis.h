/* Created by Mundanity on 23-4-8.
 * Encoding with UTF-8
 * 词法分析程序，采用 DFA 实现词法分析，即给定输入可以确定下一状态位置
 *
 * token 可由关键词，标识符，常量，限定符和运算符组成
 * */

#include <vector>
#include <string>

//// 边定义
//struct Edge{
//    // 边上符号
//    std::string vn;
//    // 边指向节点名称
//    std::string vt;
//};
//
//// 节点定义
//struct Node{
//    // 节点名称
//    std::string name;
//    // 节点包含边
//    std::vector<Edge> edges;
//};

struct Record{
    std::string start;
    std::string end;
    std::string edge;
};

// 文法G的四元组
struct Grammar{
    // VN, 非终结符集
    std::vector<std::string> VN;
    // VT, 终结符集
    std::vector<std::string> VT;
    // P, 产生式集合
    std::vector<std::string> P;
    // S, 初态集合
    std::vector<std::string> S;
    // Z, 终态集合
    std::vector<std::string> Z;
};

struct CheckMap{
    std::vector<Record> InitialNode;
    std::vector<Record> ProcessNode;
};

// 分析器类定义
class Lexical_Analysis{
private:
    // 文法产生式位置
    std::string grammar_file;

    // 文法 G 的参数
    Grammar G;

    // NFA 的图
    std::vector<Record> Net;

    //
    CheckMap Map;

public:
    // 构造函数
    Lexical_Analysis(std::string filename);

    // 析构函数
    ~Lexical_Analysis();

    // NFA 生成
    void generate();

    // 打印 NFA
    void print_NFA();

    // token 检查
    void check(std::string filename);

    // 产生式分割，将产生式划分为左部与右部
    std::vector<std::string> divide_line(const std::string& production);

    // 产生式右侧以'|'再次进行分割
    static std::vector<std::string> split_right(std::string right);

    //清除多余空格
    static void clean_space(std::string &str);

    // 是否在 VT 集合中
    bool is_in_VT(std::string target);

    // 是否在 VN 集合中
    bool is_in_VN(std::string target);

    std::string search_next_node(std::string target, std::string current_node,std::vector<Record> list);
};

