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
        nowtable = typetableRoot = new namespore();
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
                    if (child1->type == TYPE_INT_CONST || child1->type == TYPE_CHAR_CONST) {
                        cerror("const var without initialization");
                    }
                } else {
                    cerror("error define");
                }

                if (now->child->vartype == VAR_TYPE) {
                    nowtable->var[now->child->child->var_name].varsize = 4;
                    if (child1->type == TYPE_INT)
                        nowtable->var[now->child->child->var_name] = VarNode(INT);
                    else if (child1->type == TYPE_CHAR)
                        nowtable->var[now->child->child->var_name] = VarNode(CHARR);
                    else if (child1->type == TYPE_INT_CONST) {
                        if (now->child->sibling->exptype != INTEGER_VAL) {
                            cerror("can not init with not a integer");
                        }
                        nowtable->var[now->child->child->var_name].constval = now->child->sibling->int_val;
                        nowtable->var[now->child->child->var_name] = VarNode(CONST_INT);
                    } else if (child1->type == TYPE_CHAR_CONST) {
                        if (now->child->sibling->exptype != CHAR_VAL) {
                            cerror("can not init with not a char const");
                        }
                        nowtable->var[now->child->child->var_name].constval = now->child->sibling->ch_val;
                        nowtable->var[now->child->child->var_name] = VarNode(CONST_CHAR);
                    } else if (child1->type == TYPE_COMPOSE_STRUCT) {
                        if (typetableRoot->var.find(child1->var_name) == typetableRoot->var.end()) {
                            cerror("this struct not define");
                        }
                        if (typetableRoot->var[child1->var_name].basetype != STRUCT_DEF) {
                            cerror("this IDENTIFIER is not a struct");
                        }
                        nowtable->var[now->child->child->var_name] = VarNode(STRUCT);
                        nowtable->var[now->child->child->var_name].nametype = child1->var_name;
                        nowtable->var[now->child->child->var_name].varsize = typetableRoot->var[child1->var_name].varsize;
                    } else {
                        cerror("type not support");
                    }
                } else if (now->child->vartype == ARRAY_TYPE) {
                    int basesize = 4;

                    if (child1->type == TYPE_INT)
                        nowtable->var[now->child->child->var_name] = VarNode(INT_ARRAY);
                    else if (child1->type == TYPE_CHAR)
                        nowtable->var[now->child->child->var_name] = VarNode(CHAR_ARRAY);
                    else if (child1->type == TYPE_INT_CONST) {
                        cerror("not support const int array");
                        nowtable->var[now->child->child->var_name] = VarNode(CONST_INT_ARRAY);
                    } else if (child1->type == TYPE_CHAR_CONST) {
                        cerror("not support const char array");
                        nowtable->var[now->child->child->var_name] = VarNode(CONST_CHAR_ARRAY);
                    } else if (child1->type == TYPE_COMPOSE_STRUCT) {
                        if (typetableRoot->var.find(child1->var_name) == typetableRoot->var.end()) {
                            cerror("this struct not define");
                        }
                        if (typetableRoot->var[child1->var_name].basetype != STRUCT_DEF) {
                            cerror("this IDENTIFIER is not a struct");
                        }
                        basesize = typetableRoot->var[child1->var_name].varsize;
                        nowtable->var[now->child->child->var_name] = VarNode(STRUCT_ARRAY);
                        nowtable->var[now->child->child->var_name].nametype = child1->var_name;

                    } else {
                        cerror("type not support");
                    }

                    nowtable->var[now->child->child->var_name].arr_dim = now->child->child->sibling->array_dim;
                    auto arr_dim_list = now->child->child->sibling->child;//arr_dim
                    //检查声明的数组下标是否是int
                    int base_array = 1;
                    while (arr_dim_list != nullptr) {
                        if (arr_dim_list->exptype != INTEGER_VAL) {
                            cerror("arr dim should be a integer");
                        } else {
                            base_array *= arr_dim_list->int_val;
                            nowtable->var[now->child->child->var_name].dim_num.push_back(arr_dim_list->int_val);
                        }
                        arr_dim_list = arr_dim_list->sibling;
                    }
                    nowtable->var[now->child->child->var_name].varsize = base_array * basesize;
                } else {
                    cerror("not right type");
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

            for (auto x:nowtable->var) {
                nowtable->fieldsize += x.second.varsize;
            }
            nowtable = nowtable->fa;
            in_loop--;
        } else if (this->stype == STMT_FOR) {
            in_loop++;
            nowtable = nowtable->newChild();

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
            for (auto x:nowtable->var) {
                nowtable->fieldsize += x.second.varsize;
            }
            nowtable = nowtable->fa;
            in_loop--;
        } else if (this->stype == STMT_ASSIGN || this->stype == STMT_ASSIGN_ADD || this->stype == STMT_ASSIGN_MUL ||
                   this->stype == STMT_ASSIGN_SUB || this->stype == STMT_ASSIGN_DIV || this->stype == STMT_ASSIGN_MOD ||
                   this->stype == STMT_ASSIGN_ADD_SELF) {
            this->getExprType(nowtable);
        } else if (this->stype == STMT_FUNCTION_CALL) {
            if (typetableRoot->var.find(this->child->var_name) != typetableRoot->var.end()) {
                auto funcname = this->child->var_name;
                if (typetableRoot->var[funcname].basetype != FUNC) {
                    cerror("this is not a function");
                }
                if (typetableRoot->var[funcname].returnType != RETVOID) {
                    cerror("return not use");
                }
                auto temp = this->child->sibling;
                if (temp->array_dim != typetableRoot->structvar[funcname]->param_list.size()) {
                    cerror("param num not right");
                }
                temp = temp->child;
                int x = -1;
                while (temp != nullptr) {
                    if (temp->getExprType(nowtable).basetype !=
                        typetableRoot->structvar[funcname]->param_list[++x].basetype) {
                        cerror("function param type error");
                    }
                    temp = temp->sibling;
                }
            } else {
                cerror("no such function");
            }
        } else if (this->stype == STMT_IF_ELSE || this->stype == STMT_IF) {
            if (this->child->getExprType(nowtable).basetype != BOOLL) {
                cerror("try to transform other type to bool");
            }
            //检查then 语句
            nowtable = nowtable->newChild();

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
            if (this->stype == STMT_IF_ELSE) {
                //检查else语句
                nowtable = nowtable->newChild();
                auto tmp = this->child->sibling->sibling;
                if (tmp->stype == STMT_BLOCK) {
                    tmp = tmp->child;
                    while (tmp != nullptr) {
                        tmp->genTable(nowtable);
                        tmp = tmp->sibling;
                    }
                } else {
                    tmp->genTable(nowtable);
                }
                for (auto x:nowtable->var) {
                    nowtable->fieldsize += x.second.varsize;
                }
                nowtable = nowtable->fa;
            }
        } else if (this->stype == STMT_SCANF) {
            auto temp = this->child->sibling;
            if (temp != nullptr) {
                temp = temp->child;
                while (temp != nullptr) {
                    temp->getIdValType(nowtable);
                    temp = temp->sibling;
                }
            }
        } else if (this->stype == STMT_PRINT) {

            auto temp = this->child->sibling;
            if (temp != nullptr) {
                temp = temp->child;
                while (temp != nullptr) {
                    temp->getExprType(nowtable);
                    temp = temp->sibling;
                }
            }
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
            temp = temp->sibling;
            temp = temp->sibling;
        }
        nowtable->fa->structvar[function_name] = nowtable;

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
        nowtable->var[struct_name] = VarNode(STRUCT_DEF);
        nowtable = nowtable->newChild();
        nowtable->fa->structvar[struct_name] = nowtable;
        auto temp = this->child->sibling->child;

        while (temp != nullptr) {
            temp->genTable(nowtable);
            temp = temp->sibling;
        }
        for (auto &x:nowtable->var) {
            x.second.offset_struct = nowtable->fa->var[struct_name].varsize;
            nowtable->fa->var[struct_name].varsize += x.second.varsize;
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


void TreeNode::printAST(namespore *nowtable) {

    if (this->nodeType == NODE_PROG) {
        //声明全局变量
        auto temp = this->child->child;
        while (temp != nullptr) {
            if (temp->nodeType == NODE_STMT && temp->stype == STMT_DECL) {
                auto child2 = this->child->sibling;//参数列表
                auto child1 = this->child;
                for (auto now = child2->child; now != nullptr; now = now->sibling) {
                    //now define_with_init
                    //now child IDENTIFIER_val
                    //now child child IDENTIFIER
                    //类型检查，查看是否赋值初始化符合逻辑
                    if (now->ftype == DEFINE_FORMAT_INIT) {
                        auto exprtype = now->child->sibling->getExprType(nowtable);
                        if ((child1->type == TYPE_INT || child1->type == TYPE_INT_CONST)) {
                            if (now->child->sibling->exptype != INTEGER_VAL) {
                                cerror("int gobal var should be init with int");
                            }
                        } else if ((child1->type == TYPE_CHAR || child1->type == TYPE_CHAR_CONST)) {
                            if (now->child->sibling->exptype != CHAR_VAL) {
                                cerror("char gobal var should be init with char");
                            }
                        }
                    }


                    if (now->child->vartype == VAR_TYPE) {
                        if (child1->type == TYPE_INT || )
                            nowtable->var[now->child->child->var_name] = VarNode(INT);
                        else if (child1->type == TYPE_CHAR)
                            nowtable->var[now->child->child->var_name] = VarNode(CHARR);
                        else if (child1->type == TYPE_INT_CONST) {
                            if (now->child->sibling->exptype != INTEGER_VAL) {
                                cerror("can not init with not a integer");
                            }
                            nowtable->var[now->child->child->var_name].constval = now->child->sibling->int_val;
                            nowtable->var[now->child->child->var_name] = VarNode(CONST_INT);
                        } else if (child1->type == TYPE_CHAR_CONST) {
                            if (now->child->sibling->exptype != CHAR_VAL) {
                                cerror("can not init with not a char const");
                            }
                            nowtable->var[now->child->child->var_name].constval = now->child->sibling->ch_val;
                            nowtable->var[now->child->child->var_name] = VarNode(CONST_CHAR);
                        } else if (child1->type == TYPE_COMPOSE_STRUCT) {
                            if (typetableRoot->var.find(child1->var_name) == typetableRoot->var.end()) {
                                cerror("this struct not define");
                            }
                            if (typetableRoot->var[child1->var_name].basetype != STRUCT_DEF) {
                                cerror("this IDENTIFIER is not a struct");
                            }
                            nowtable->var[now->child->child->var_name] = VarNode(STRUCT);
                            nowtable->var[now->child->child->var_name].nametype = child1->var_name;
                            nowtable->var[now->child->child->var_name].varsize = typetableRoot->var[child1->var_name].varsize;
                        } else {
                            cerror("type not support");
                        }
                    } else if (now->child->vartype == ARRAY_TYPE) {
                        int basesize = 4;

                        if (child1->type == TYPE_INT)
                            nowtable->var[now->child->child->var_name] = VarNode(INT_ARRAY);
                        else if (child1->type == TYPE_CHAR)
                            nowtable->var[now->child->child->var_name] = VarNode(CHAR_ARRAY);
                        else if (child1->type == TYPE_INT_CONST) {
                            cerror("not support const int array");
                            nowtable->var[now->child->child->var_name] = VarNode(CONST_INT_ARRAY);
                        } else if (child1->type == TYPE_CHAR_CONST) {
                            cerror("not support const char array");
                            nowtable->var[now->child->child->var_name] = VarNode(CONST_CHAR_ARRAY);
                        } else if (child1->type == TYPE_COMPOSE_STRUCT) {
                            if (typetableRoot->var.find(child1->var_name) == typetableRoot->var.end()) {
                                cerror("this struct not define");
                            }
                            if (typetableRoot->var[child1->var_name].basetype != STRUCT_DEF) {
                                cerror("this IDENTIFIER is not a struct");
                            }
                            basesize = typetableRoot->var[child1->var_name].varsize;
                            nowtable->var[now->child->child->var_name] = VarNode(STRUCT_ARRAY);
                            nowtable->var[now->child->child->var_name].nametype = child1->var_name;

                        } else {
                            cerror("type not support");
                        }

                        nowtable->var[now->child->child->var_name].arr_dim = now->child->child->sibling->array_dim;
                        auto arr_dim_list = now->child->child->sibling->child;//arr_dim
                        //检查声明的数组下标是否是int
                        int base_array = 1;
                        while (arr_dim_list != nullptr) {
                            if (arr_dim_list->exptype != INTEGER_VAL) {
                                cerror("arr dim should be a integer");
                            } else {
                                base_array *= arr_dim_list->int_val;
                                nowtable->var[now->child->child->var_name].dim_num.push_back(arr_dim_list->int_val);
                            }
                            arr_dim_list = arr_dim_list->sibling;
                        }
                        nowtable->var[now->child->child->var_name].varsize = base_array * basesize;
                    } else {
                        cerror("not right type");
                    }
                }

            }
            temp = temp->sibling;
        }
    } else if (this->nodeType == NODE_STMT) {
        if (in_function == "") {
            return;
        }
    } else if (this->nodeType == NODE_FUNC) {

    } else if (this->nodeType == NODE_STRUCT) {

    } else {
        cerror("not support");
    }
}


VarNode TreeNode::getIdValType(namespore *nowtable) {
    //todo
    if (this->vartype == VAR_TYPE) {
        auto id_name = this->child->var_name;

        auto temptable = nowtable;
        while (temptable != nullptr && temptable->var.find(id_name) == temptable->var.end()) {
            temptable = temptable->fa;
        }

        if (temptable == nullptr || temptable->var[id_name].arr_dim != this->child->array_dim) {
            cerror("can not find this IDENTIFIER");
        }
        if (temptable->var[id_name].basetype == INT || temptable->var[id_name].basetype == CHARR) {
            return VarNode(temptable->var[id_name].basetype);
        } else if (temptable->var[id_name].basetype == CONST_INT) {
            auto tempret = VarNode(INT);
            tempret.returnType = CONST_INT;
            return tempret;
        } else if (temptable->var[id_name].basetype == CONST_CHAR) {
            auto tempret = VarNode(CHARR);
            tempret.returnType = CONST_CHAR;
            return tempret;
        } else {
            cerror("this is not a right IDENTIFIER");
        }
    } else if (this->vartype == ARRAY_TYPE) {
        auto id_name = this->child->var_name;
        auto temptable = nowtable;
        while (temptable != nullptr && temptable->var.find(id_name) == temptable->var.end()) {
            temptable = temptable->fa;
        }
        if (temptable == nullptr || temptable->var[id_name].arr_dim != this->child->sibling->array_dim) {
            cerror("can not find this IDENTIFIER");
        }

        auto dimtemp = this->child->sibling->child;
        while (dimtemp != nullptr) {
            if (dimtemp->getExprType(nowtable).basetype != INT) {
                cerror("array dim should be a integer");
            }
            dimtemp = dimtemp->sibling;
        }

        if (temptable->var[id_name].basetype == INT_ARRAY) {
            return VarNode(INT);

        } else if (temptable->var[id_name].basetype == CHAR_ARRAY) {
            return VarNode(CHARR);
        } else if (temptable->var[id_name].basetype == CONST_INT_ARRAY) {
            auto tempret = VarNode(INT);
            tempret.returnType = CONST_INT;
            return tempret;
        } else if (temptable->var[id_name].basetype == CONST_CHAR_ARRAY) {
            auto tempret = VarNode(CHARR);
            tempret.returnType = CONST_CHAR;
            return tempret;
        } else {
            cerror("this is not a right IDENTIFIER");
        }
    } else if (this->vartype == STRUCT_TYPE) {
        if ((this->child->vartype != VAR_TYPE && this->child->vartype != ARRAY_TYPE) ||
            (this->child->sibling->vartype != VAR_TYPE && this->child->sibling->vartype != ARRAY_TYPE)) {
            cerror("struct nest not support");
        }


        auto id_name = this->child->var_name;
        auto temptable = nowtable;
        while (temptable != nullptr && temptable->var.find(id_name) == temptable->var.end()) {
            temptable = temptable->fa;
        }
        if (temptable == nullptr ||
            (temptable->var[id_name].basetype != STRUCT && temptable->var[id_name].basetype != STRUCT_ARRAY) ||
            temptable->var[id_name].arr_dim != this->child->array_dim) {
            cerror("can not find this IDENTIFIER");
        }
        auto tempspore = typetableRoot->structvar[temptable->var[id_name].nametype];
        if (tempspore->var.find(this->child->sibling->var_name) == tempspore->var.end()) {
            cerror("this struct does not have such member");
        }
        if ((tempspore->var[this->child->sibling->var_name].basetype == INT ||
             tempspore->var[this->child->sibling->var_name].basetype == CHARR
             || tempspore->var[this->child->sibling->var_name].basetype == INT_ARRAY ||
             tempspore->var[this->child->sibling->var_name].basetype == CHAR_ARRAY) &&
            (tempspore->var[this->child->sibling->var_name].arr_dim == this->child->sibling->array_dim)) {
            //todo
        } else {
            cerror("struct member is not right");
        }
        //todo:维度下标检查


        if (tempspore->var[this->child->sibling->var_name].basetype == INT ||
            tempspore->var[this->child->sibling->var_name].basetype == INT_ARRAY)
            return VarNode(INT);
        else if (tempspore->var[this->child->sibling->var_name].basetype == CHARR ||
                 tempspore->var[this->child->sibling->var_name].basetype == CHAR_ARRAY)
            return VarNode(CHARR);
        else {
            cerror("not support");
        }
    } else {
        cerror("not support this type");
    }
}

VarNode TreeNode::getExprType(namespore *nowtable) {
    if (this->
            nodeType == NODE_STMT) {
        auto tmp1 = this->child->getIdValType(nowtable);
        if (tmp1.returnType == CONST_INT || tmp1.returnType == CONST_CHAR) {
            cerror("const var can not assgin");
        }

        if (this->child->sibling != nullptr) {
            auto tmp2 = this->child->sibling->getExprType(nowtable);
            if (tmp2.basetype != tmp1.basetype) {
                cerror("only same type can assgin each other");
            }
        }
        return VarNode(tmp1.basetype);
    } else if (this->nodeType == NODE_EXPR) {

        if (this->exptype == INTEGER_VAL) {
            return VarNode(INT);
        } else if (this->exptype == CHAR_VAL)
            return VarNode(CHARR);
        else if (this->exptype == STRING_VAL) {
            cerror("not support string in calculate");
        } else if (this->exptype == IDENTIFIER_VAL) {
            return VarNode(this->getIdValType(nowtable));
        } else if (this->exptype == FUNC_CALL) {
            if (typetableRoot->var.find(this->child->var_name) != typetableRoot->var.end()) {
                auto funcname = this->child->var_name;
                if (typetableRoot->var[funcname].basetype != FUNC) {
                    cerror("this is not a function");
                }

                auto temp = this->child->sibling;
                if (temp->array_dim != typetableRoot->structvar[funcname]->param_list.size()) {
                    cerror("param num not right");
                }
                temp = temp->child;
                int x = -1;
                while (temp != nullptr) {
                    if (temp->getExprType(nowtable).basetype !=
                        typetableRoot->structvar[funcname]->param_list[++x].basetype) {
                        cerror("function param type error");
                    }
                    temp = temp->sibling;
                }
                return VarNode(typetableRoot->var[this->child->var_name].returnType);
            } else {
                cerror("no such function");
            }
        } else if (this->exptype == OR_BOOL || this->exptype == AND_BOOL) {
            auto tmp1 = this->child->getExprType(nowtable);
            auto tmp2 = this->child->sibling->getExprType(nowtable);
            if (tmp2.basetype != BOOLL || tmp1.basetype != BOOLL) {
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
            return VarNode(BOOLL);
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

VarNode::VarNode(Basetype
                 a = INT) {
    this->basetype = a;
}

void TreeNode::cerror(const char *mess) {
    cout << mess << endl;
    exit(1);
}