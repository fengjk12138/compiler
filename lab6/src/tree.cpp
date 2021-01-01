#include "tree.h"

using namespace std;

void TreeNode::addChild(TreeNode *child) {
    if (this->child == nullptr) {
        this->child = child;
    } else {
        TreeNode *tmp = this->child;
        while (tmp->sibling != nullptr) {
            tmp = tmp->sibling;
        }
        tmp->sibling = child;
    }
}

void TreeNode::addSibling(TreeNode *sibling) {
    TreeNode *tmp = this;
    while (tmp->sibling != nullptr) {
        tmp = tmp->sibling;
    }
    tmp->sibling = sibling;
}

TreeNode::TreeNode(int lineno, NodeType type) {
    this->lineno = lineno;
    this->nodeType = type;
}

bool in_function = 0;

void TreeNode::genTable(namespore *nowtable) {
    if (this->nodeType == NODE_PROG) {
        this->child->genTable(nowtable);
    } else if (this->nodeType == NODE_STMT) {
        if (this->stype == STMT_DECL) {
            auto child2 = this->child->sibling;//参数列表
            auto child1 = this->child;
            for (auto now = child2->child; now != nullptr; now = now->sibling) {
                {
                    //now define list
                    //now child define_with_init
                    //now child child IDENTIFIER
                    if (nowtable->var.find(now->child->child->var_name) != nowtable->var.end()) {
                        yyerror("Repeated definition");
                    }
                    if (now->child->vartype == VAR_TYPE) {
                        if (child1->type == TYPE_INT)
                            nowtable->var[now->child] = VarNode(INT);
                        else if (child1->type == TYPE_CHAR)
                            nowtable->var[now->child] = VarNode(CHAR);
                        else if (child1->type == TYPE_INT_CONST)
                            nowtable->var[now->child] = VarNode(CONST_INT);
                        else if (child1->type == TYPE_CHAR_CONST)
                            nowtable->var[now->child] = VarNode(CONST_CHAR);
                    } else if (now->child->vartype == ARRAY_TYPE) {
                        if (child1->type == TYPE_INT)
                            nowtable->var[now->child] = VarNode(INT_ARRAY);
                        else if (child1->type == TYPE_CHAR)
                            nowtable->var[now->child] = VarNode(CHAR_ARRAY);
                        else if (child1->type == TYPE_INT_CONST)
                            nowtable->var[now->child] = VarNode(CONST_INT_ARRAY);
                        else if (child1->type == TYPE_CHAR_CONST)
                            nowtable->var[now->child] = VarNode(CONST_CHAR_ARRAY);
                        nowtable->var[now->child].arr_dim = now->child->child->sibling->array_dim;
                    } else {
                        yyerror("not right type");
                    }
                }
            }
        }
    } else if (this->nodeType == NODE_FUNC) {
        in_function = 1;
        string function_name = this->child->sibling->var_name;
        if (nowtable->var.find(function_name) != nowtable->var.end()) {
            yyerror("Repeated definition");
        }
        nowtable->var[function_name] = VarNode(FUNC);
        if(this->child->type==TYPE_INT){
            nowtable->var[function_name].returnType=INT;
        }else if(this->child->type==TYPE_CHAR){
            nowtable->var[function_name].returnType=CHAR;
        }else{
            yyerror("function return in these type not implement");
        }
        nowtable = nowtable->newChild();
        //参数列表
        auto temp=this->child->sibling->sibling;



        //函数体
        auto tmp = this->child->sibling->sibling->sibling->child;
        while (tmp != nullptr) {
            genTable(nowtable);
            tmp = tmp->sibling;
        }
        nowtable = nowtable->fa;
        in_function = 0;
    } else if (this->nodeType == NODE_STRUCT) {

    }


}

namespore *namespore::newChild() {
    if (this->child == nullptr) {
        this->child = new namespore();
        this->child->fa = this;
        return this->child;
    } else {
        auto tmp = this->child;
        while (tmp->sibling != nullptr)
            tmp = tmp->sibling;
        tmp->sibling = new namespore();
        tmp->sibling->fa = this;
        return tmp->sibling;
    }
}


void TreeNode::printAST() {

}