/* Created by Mundanity on 23-4-8.
 * Encoding with UTF-8
 * 词法分析程序，采用 DFA 实现词法分析，即给定输入可以确定下一状态位置
 *
 * token 可由关键词，标识符，常量，限定符和运算符组成
 * */

#include <vector>
#include <string>

// 一个边的记录
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

// 将 NFA 的图分为初始节点边与过程节点边两部分
struct CheckMap{
    std::vector<Record> InitialNode;
    std::vector<Record> ProcessNode;
};

// 一个判断的结果
struct Result{
    int line_number;
    std::string type;
    std::string token;
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

    // 由 NFA 生成的
    CheckMap Map;

    // 结果列表
    std::vector<Result> Result_list;

    // 保留标识符定义
    std::vector<std::string> Reserved_Identifier = {
            "int",
            "real",
            "string",
            "boolean",
            "complex",
            "function",
            "VAR",
            "var",
            "begin",
            "BEGIN",
            "end",
            "END",
            "for",
            "while",
            "do",
            "if",
            "else",
            "repeat",
            "until",
    };
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

    // 查询队列
    void start_search(char initial, std::string &current_node, int &index, int max_length, std::string line, int line_number, std::string start);

    // 是否为表留标识符
    bool is_reserved(std::string target);

    // 获取一个节点所有可能的边
    std::vector<std::string> get_available_token(std::string current_node);

    // content 是否包含在 list当中
    bool is_contain(std::string content, std::vector<std::string> list);

    // 打印结果
    void print_result();

    // 输出结果
    void save_result();
};

