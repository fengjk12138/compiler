#ifndef TREE_H
#define TREE_H

#include "pch.h"
#include "type.h"

enum NodeType {
    NODE_CONST,
    NODE_VAR,
    NODE_EXPR,
    NODE_TYPE,


    NODE_FUNC,
    NODE_STRUCT,
    NODE_FORMT, //递归列表的一种范式
    NODE_STMT,
    NODE_PROG, //整个程序
    NODE_EMPTY, //空语句，用于维持整个语法树的规整
};

enum ForMat {
    DEFINE_LIST,
    PARAM_LIST,
    PARAM_LIST_CALL,
    ARRAY_DIM,
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

    POINTER,//指针
    ADDRESS,//取地址

    FUNC_CALL,
    IDENTIFIER_VAL,
    INTEGER_VAL,
    CHAR_VAL,
    STRING_VAL,
};

enum VarType {
    ARRAY_TYPE,
    STRUCT_TYPE,
    VAR_TYPE,
    CONST_BOOL_TYPE,
    CONST_INT_TYPE,
    CONST_CHAR_TYPE,
    CONST_STRING_TYPE,

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
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_FUNCTION_CALL,
    STMT_FUNCTION_CALL_GIVEN,
    STMT_SCANF,
    STMT_PRINT,
    STMT_MAIN,
    STMT_WHILE,
    STMT_IF,
    STMT_IF_ELSE,
    STMT_FOR,
};
enum Basetype {
    INT,
    CHARR,
    CONST_INT,
    CONST_CHAR,
    CONST_INT_ARRAY,
    CONST_CHAR_ARRAY,
    INT_ARRAY,
    CHAR_ARRAY,
    STRUCT,
    STRUCT_ARRAY,
    FUNC,
    BOOLL,
    RETVOID,
    STRUCT_DEF,
};
enum Spetype {
    STRUCT_VAR,
    FUNC_VAR,
};

struct VarNode {
    int pos;
    int arr_dim = 0;
    std::vector<int> dim_num;
    Basetype returnType;
    Spetype spetype;
    Basetype basetype;
    string nametype;

    VarNode(Basetype);
};

struct namespore {
    std::vector <VarNode> param_list;
    namespore *fa = nullptr;
    std::map <std::string, VarNode> var;
    std::map<std::string, namespore *> structvar;
    namespore *child = nullptr;
    namespore *sibling = nullptr;

    namespore *newChild();

};

struct TreeNode {
public:
    int lineno;
    NodeType nodeType;

    TreeNode *child = nullptr;
    TreeNode *sibling = nullptr;

    void addChild(TreeNode *);

    VarNode getExprType(namespore *);

    VarNode getIdValType(namespore *);

    void addSibling(TreeNode *);

    static void cerror(const char *mess);

    void printAST(); // 先输出自己 + 孩子们的id；再依次让每个孩子输出AST。
    void genTable(namespore *);

public:
    ExpType exptype;

    StmtType stype;
    ForMat ftype;


    //常量使用
    int var_id;
    int int_val;
    char ch_val;
    bool b_val;
    string str_val;

    //变量使用
    Type *type;  // int void char变量、类型、表达式结点，有类型。
    int array_dim = 0;
    VarType vartype; //变量(常量)的类型
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


#endif