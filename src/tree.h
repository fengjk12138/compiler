#ifndef TREE_H
#define TREE_H

#include "pch.h"
#include "type.h"

enum NodeType {
    NODE_CONST,
    NODE_VAR,
    NODE_EXPR,
    NODE_TYPE,

    NODE_FORMT,
    NODE_STMT,
    NODE_PROG,
};

enum ForMat{
    SCANF_FORMAT_ADDR,
    SCANF_FORMAT,
    DEFINE_FORMAT_INIT,
    DEFINE_FORMAT,
    PRINT_FORMAT,
};

enum ExpType {
    NOTEQL,// !=
    EQL, //==
    BIGEQL,//>=
    SMALLEQL, //<=
    BIG,//>
    SMALL,//<

    AND_BOOL,// &&
    OR_BOOL,// ||
    NOT_BOOL,// !

    ADD,// +
    SUB,// -
    MUL,// *
    DIV,// /
    MOD,// %
    //ADD_SELF, //++

    BR,// ()

    POS,// +4
    NEG,// -5
};

enum StmtType {
    STMT_SKIP,
    STMT_DECL,
    STMT_ASSIGN,
    STMT_ASSIGN_ADD,
    STMT_ASSIGN_SUB,
    STMT_ASSIGN_MUL,
    STMT_ASSIGN_DIV,
    STMT_ASSIGN_MOD,
    STMT_ASSIGN_ADD_SELF,
    STMT_BLOCK,
    STMT_RET,
    STMT_SCANF,
    STMT_PRINT,
    STMT_MAIN,
    STMT_WHILE,
    STMT_IF,
    STMT_IF_ELSE,
    STMT_FOR,
};

struct TreeNode {
public:
    static int nowID;
    int nodeID;  // 用于作业的序号输出
    int lineno;
    NodeType nodeType;

    TreeNode *child = nullptr;
    TreeNode *sibling = nullptr;

    void addChild(TreeNode *);

    void addSibling(TreeNode *);

    void printNodeInfo();

    void printChildrenId();

    void printAST(); // 先输出自己 + 孩子们的id；再依次让每个孩子输出AST。
    void printSpecialInfo();

    void genNodeId();

public:
    ExpType exptype;
    Type *type;  // 变量、类型、表达式结点，有类型。
    StmtType stype;
    ForMat ftype;
    int var_id;
    int int_val;
    char ch_val;
    bool b_val;
    string str_val;
    string var_name;
//public:
//    static string nodeType2String(NodeType type);
//
//    static string opType2String(OperatorType type);
//
//    static string sType2String(StmtType type);

public:
    TreeNode(int lineno, NodeType type);
};

struct VarNode {
    static int nodeID;
    int lineno;
    VarNode *fa = nullptr;
    std::map <std::string, std::pair<int, Type *>> var;
    VarNode *child = nullptr;
    VarNode *sibling = nullptr;

    void addChild(VarNode *);
};

#endif