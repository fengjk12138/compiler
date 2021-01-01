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
        nowtable = new namespore();
        auto tmp = this->child->child;
        while (tmp != nullptr) {
            genTable(nowtable);
            tmp = tmp->sibling;
        }
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
                        cerror("Repeated definition");
                    }
                    if(now->child->ftype==DEFINE_FORMAT_INIT){
                        auto exprtype=getExprType(nowtable);
                        if(exprtype.basetype!=)//todo:

                    }else if(now->child->ftype==DEFINE_FORMAT){

                    }else{
                        cerror("error define");
                    }


                    if (now->child->vartype == VAR_TYPE) {
                        if (child1->type == TYPE_INT)
                            nowtable->var[now->child->child->var_name] = VarNode(INT);
                        else if (child1->type == TYPE_CHAR)
                            nowtable->var[now->child->child->var_name] = VarNode(CHARR);
                        else if (child1->type == TYPE_INT_CONST)
                            nowtable->var[now->child->child->var_name] = VarNode(CONST_INT);
                        else if (child1->type == TYPE_CHAR_CONST)
                            nowtable->var[now->child->child->var_name] = VarNode(CONST_CHAR);
                    } else if (now->child->vartype == ARRAY_TYPE) {
                        if (child1->type == TYPE_INT)
                            nowtable->var[now->child->child->var_name] = VarNode(INT_ARRAY);
                        else if (child1->type == TYPE_CHAR)
                            nowtable->var[now->child->child->var_name] = VarNode(CHAR_ARRAY);
                        else if (child1->type == TYPE_INT_CONST)
                            nowtable->var[now->child->child->var_name] = VarNode(CONST_INT_ARRAY);
                        else if (child1->type == TYPE_CHAR_CONST)
                            nowtable->var[now->child->child->var_name] = VarNode(CONST_CHAR_ARRAY);
                        nowtable->var[now->child->child->var_name].arr_dim = now->child->child->sibling->array_dim;



                    } else {
                        cerror("not right type");
                    }
                }
            }
        } else if (this->stype == STMT_BREAK) {

        } else if (this->stype == STMT_RET) {
            if(in_function==0){
                cerror("return should be used in function");
            }
            if(this->child->nodeType!=NODE_EMPTY){

            }
        } else if (this->stype == STMT_SKIP) {

        } else if (this->stype == STMT_CONTINUE) {

        }
    } else if (this->nodeType == NODE_FUNC) {
        in_function = 1;
        string function_name = this->child->sibling->var_name;
        if (nowtable->var.find(function_name) != nowtable->var.end()) {
            cerror("Repeated definition");
        }
        nowtable->var[function_name] = VarNode(FUNC);
        if (this->child->type == TYPE_INT) {
            nowtable->var[function_name].returnType = INT;
        } else if (this->child->type == TYPE_CHAR) {
            nowtable->var[function_name].returnType = CHARR;
        } else {
            cerror("function return these type not implement");
        }
        nowtable->var[function_name].basetype = FUNC;
        nowtable = nowtable->newChild();
        //参数列表
        auto temp = this->child->sibling->sibling->child;

        while (temp != nullptr) {
            auto para_name = temp->sibling->var_name;
            if (nowtable->var.find(para_name) != nowtable->var.end()) {
                cerror("Repeated definition");
            }
            if (temp->type == TYPE_INT) {
                nowtable->var[para_name] = VarNode(INT);
            } else if (temp->type == TYPE_CHAR) {
                nowtable->var[para_name] = VarNode(CHARR);
            } else {
                cerror("function parameter these type not implement");
            }
            nowtable->param_list.push_back(nowtable->var[para_name]);
        }

        //函数体
        auto tmp = this->child->sibling->sibling->sibling->child;
        while (tmp != nullptr) {
            genTable(nowtable);
            tmp = tmp->sibling;
        }
        nowtable = nowtable->fa;
        in_function = 0;
    } else if (this->nodeType == NODE_STRUCT) {
        auto struct_name = this->var_name;
        if (nowtable->var.find(struct_name) != nowtable->var.end()) {
            cerror("Repeated definition");
        }
        nowtable = nowtable->newChild();

        auto temp = this->child->sibling->child;
        while (temp != nullptr) {
            genTable(nowtable);
            temp = temp->sibling;
        }
        nowtable = nowtable->fa;
    } else {
        cerror("your code has something can not parse")
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
VarNode TreeNode::getExprType(namespore *nowtable){

}
bool namespore::findExist() {

}

VarNode::VarNode(Basetype a = INT) {
    this->basetype = a;
}

void TreeNode::cerror(const char *mess) {
    cout << mess << endl;
    exit(1);
}