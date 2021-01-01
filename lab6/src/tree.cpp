#include "tree.h"

namespore *typetableRoot;
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

string in_function = "";
int in_loop = 0;

void TreeNode::genTable(namespore *nowtable) {
    if (this->nodeType == NODE_PROG) {
        nowtable = new namespore();
        auto tmp = this->child->child;
        while (tmp != nullptr) {
            tmp->genTable(nowtable);
            tmp = tmp->sibling;
        }
    } else if (this->nodeType == NODE_STMT) {
        if (this->stype == STMT_DECL) {
            auto child2 = this->child->sibling;//参数列表
            auto child1 = this->child;
            for (auto now = child2->child; now != nullptr; now = now->sibling) {
                {
                    //now define_with_init
                    //now child IDENTIFIER_val
                    //now child child IDENTIFIER
                    if (nowtable->var.find(now->child->child->var_name) != nowtable->var.end()) {
                        cerror("Repeated definition");
                    }
                    //类型检查，查看是否赋值初始化符合逻辑
                    if (now->ftype == DEFINE_FORMAT_INIT) {
                        auto exprtype = now->child->sibling->getExprType(nowtable);
                        if ((exprtype.basetype == INT &&
                             (child1->type == TYPE_INT || child1->type == TYPE_INT_CONST)) ||
                            (exprtype.basetype == CHARR &&
                             (child1->type == TYPE_CHAR || child1->type == TYPE_CHAR_CONST))) {
                            //todo
                        } else cerror("type transform not support");

                    } else if (now->ftype == DEFINE_FORMAT) {
                        if (child1->type == TYPE_INT_CONST || TYPE_CHAR_CONST) {
                            cerror("const var without initialization");
                        }
                    } else {
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
                        else if (child1->type == TYPE_COMPOSE_STRUCT) {
                            nowtable->var[now->child->child->var_name] = VarNode(STRUCT);
                            nowtable->var[now->child->child->var_name].nametype = child1->var_name;
                        } else {
                            cerror("type not support");
                        }
                    } else if (now->child->vartype == ARRAY_TYPE) {
                        if (child1->type == TYPE_INT)
                            nowtable->var[now->child->child->var_name] = VarNode(INT_ARRAY);
                        else if (child1->type == TYPE_CHAR)
                            nowtable->var[now->child->child->var_name] = VarNode(CHAR_ARRAY);
                        else if (child1->type == TYPE_INT_CONST)
                            nowtable->var[now->child->child->var_name] = VarNode(CONST_INT_ARRAY);
                        else if (child1->type == TYPE_CHAR_CONST)
                            nowtable->var[now->child->child->var_name] = VarNode(CONST_CHAR_ARRAY);
                        else if (child1->type == TYPE_COMPOSE_STRUCT) {
                            nowtable->var[now->child->child->var_name] = VarNode(STRUCT_ARRAY);
                            nowtable->var[now->child->child->var_name].nametype = child1->var_name;
                        } else {
                            cerror("type not support");
                        }
                        nowtable->var[now->child->child->var_name].arr_dim = now->child->child->sibling->array_dim;
                        auto arr_dim_list = now->child->child->sibling->child;//arr_dim
                        //检查声明的数组下标是否是int
                        while (arr_dim_list != nullptr) {
                            if (arr_dim_list->exptype != INTEGER_VAL) {
                                cerror("arr dim should be a integer");
                            } else {
                                nowtable->var[now->child->child->var_name].dim_num.push_back(arr_dim_list->int_val);
                            }
                        }
                    } else {
                        cerror("not right type");
                    }
                }
            }
        } else if (this->stype == STMT_BREAK) {
            if (in_loop == 0) {
                cerror("break should be used in loop");
            }
        } else if (this->stype == STMT_RET) {
            if (in_function == "") {
                cerror("return should be used in function");
            }
            //返回值类型检查
            if (this->child->nodeType != NODE_EMPTY) {
                auto exprtype = this->child->getExprType(nowtable);
                auto func_var = typetableRoot->var[in_function].returnType;
                if (exprtype.basetype != func_var) {
                    cerror("return type can not adapt");
                }
            } else {
                auto func_var = typetableRoot->var[in_function].returnType;
                if (func_var != RETVOID) {
                    cerror("return type can not adapt");
                }
            }
        } else if (this->stype == STMT_SKIP) {
        } else if (this->stype == STMT_CONTINUE) {
            if (in_loop == 0) {
                cerror("break should be used in loop");
            }
        } else if (this->stype == STMT_WHILE) {
            in_loop++;
            nowtable = nowtable->newChild();

            auto exptype = this->child->getExprType(nowtable);
            if (exptype.basetype != BOOLL) {
                cerror("your try to transform a type to bool, now not support");
            }
            auto tmp = this->child->sibling;
            if (tmp->stype == STMT_BLOCK) {
                tmp = tmp->child;
                while (tmp != nullptr) {
                    tmp->genTable(nowtable);
                    tmp = tmp->sibling;
                }
            } else {
                tmp->genTable(nowtable);
            }
            nowtable = nowtable->fa;
            in_loop--;
        } else if (this->stype == STMT_FOR) {
            in_loop++;

            if (this->child->nodeType != NODE_EMPTY) {
                this->child->genTable(nowtable);
            }
            if (this->child->sibling->nodeType != NODE_EMPTY) {
                auto exptype = this->child->sibling->getExprType(nowtable);
                if (exptype.basetype != BOOLL) {
                    cerror("your try to transform a type to bool, now not support");
                }
            }
            if (this->child->sibling->sibling->nodeType != NODE_EMPTY) {
                this->child->sibling->sibling->genTable(nowtable);
            }

            auto tmp = this->child->sibling->sibling->sibling;
            if (tmp->stype == STMT_BLOCK) {
                tmp = tmp->child;
                while (tmp != nullptr) {
                    tmp->genTable(nowtable);
                    tmp = tmp->sibling;
                }
            } else {
                tmp->genTable(nowtable);
            }
            nowtable = nowtable->fa;
            in_loop--;
        }
    } else if (this->nodeType == NODE_FUNC) {

        string function_name = this->child->sibling->var_name;
        in_function = function_name;
        if (nowtable->var.find(function_name) != nowtable->var.end()) {
            cerror("Repeated definition");
        }
        nowtable->var[function_name] = VarNode(FUNC);
        if (this->child->type == TYPE_INT) {
            nowtable->var[function_name].returnType = INT;
        } else if (this->child->type == TYPE_CHAR) {
            nowtable->var[function_name].returnType = CHARR;
        } else if (this->child->type == TYPE_VOID) {
            nowtable->var[function_name].returnType = RETVOID;
        } else {
            cerror("function return these type not implement");
        }
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
            tmp->genTable(nowtable);
            tmp = tmp->sibling;
        }
        nowtable = nowtable->fa;
        in_function = "";
    } else if (this->nodeType == NODE_STRUCT) {
        auto struct_name = this->var_name;
        if (nowtable->var.find(struct_name) != nowtable->var.end()) {
            cerror("Repeated definition");
        }
        nowtable = nowtable->newChild();

        auto temp = this->child->sibling->child;
        while (temp != nullptr) {
            temp->genTable(nowtable);
            temp = temp->sibling;
        }
        nowtable = nowtable->fa;
    } else {
        cerror("your code has something can not parse");
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


VarNode TreeNode::getIdValType(namespore *nowtable) {
    //todo
}

VarNode TreeNode::getExprType(namespore *nowtable) {
    if (this->nodeType == NODE_STMT) {
        auto tmp1 = this->child->getIdValType(nowtable);
        if (this->child->sibling != nullptr) {
            auto tmp2 = this->child->sibling->getExprType(nowtable);
            if (tmp2.basetype != tmp1.basetype) {
                cerror("only same type can assgin each other");
            }
        }
        return VarNode(tmp1.basetype);
    } else if (this->nodeType == NODE_EXPR) {

        if (this->exptype == INTEGER_VAL)
            return VarNode(INT);
        else if (this->exptype == CHAR_VAL)
            return VarNode(CHARR);
        else if (this->exptype == STRING_VAL) {
            cerror("not support string in calculate");
        } else if (this->exptype == IDENTIFIER_VAL) {
            return VarNode(this->getIdValType(nowtable));
        } else if (this->exptype == FUNC_CALL) {
            if (typetableRoot->var.find(this->child->var_name) != typetableRoot->var.end()) {
                return VarNode(typetableRoot->var[this->child->var_name].returntype);
            } else {
                cerror("no such function");
            }
        } else if (this->exptype == OR_BOOL || this->exptype == AND_BOOL) {
            auto tmp1 = this->child->getExprType(nowtable);
            auto tmp2 = this->child->sibling->getExprType(nowtable);
            if (tmp1.basetype != tmp2.basetype || tmp1.basetype != BOOLL) {
                cerror("or / and calculate only be used in bool");
            }
            return VarNode(BOOLL);
        } else if (this->exptype == NOT_BOOL) {
            auto tmp1 = this->child->getExprType(nowtable);
            if (tmp1.basetype != BOOLL) {
                cerror("NOT calculate only be used in bool");
            }
            return VarNode(BOOLL);
        } else if (this->exptype == EQL || this->exptype == NOTEQL || this->exptype == BIGEQL ||
                   this->exptype == SMALLEQL || this->exptype == BIG ||
                   this->exptype == SMALL) {
            auto tmp1 = this->child->getExprType(nowtable);
            auto tmp2 = this->child->sibling->getExprType(nowtable);
            if (tmp1.basetype != tmp2.basetype) {
                cerror("compera only be used in same type");
            }
            return VarNode(tmp1.basetype);
        } else if (this->exptype == ADD || this->exptype == SUB || this->exptype == MUL || this->exptype == DIV ||
                   this->exptype == MOD) {
            auto tmp1 = this->child->getExprType(nowtable);
            auto tmp2 = this->child->sibling->getExprType(nowtable);
            if (tmp1.basetype != tmp2.basetype || tmp1.basetype != INT) {
                cerror("calculate only be used in integer");
            }
            return VarNode(INT);
        } else if (this->exptype == NEG || this->exptype == POS) {
            auto tmp1 = this->child->getExprType(nowtable);
            if (tmp1.basetype != INT) {
                cerror("only integer can be signed");
            }
            return VarNode(INT);
        }
    } else {
        cerror("this is not expr");
    }
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