#include "tree.h"
#include <algorithm>

namespore *typetableRoot;
using namespace std;
int label_num = 0;
stack <pair<string, string>> loop_flag;

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
int nowpos = 4;
int timestamp = 1;
int instruct = 0;

void TreeNode::genTable(namespore *nowtable) {
    if (this->nodeType == NODE_PROG) {
        cout << ".section .rodata" << endl;
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
                    if (child1->type == TYPE_INT) {
                        nowtable->var[now->child->child->var_name] = VarNode(INT);
                        nowtable->var[now->child->child->var_name].pos = nowpos;
                        nowpos += 4;
                        if (now->child->sibling != nullptr)
                            nowtable->var[now->child->child->var_name].constval = now->child->sibling->int_val;
                        nowtable->var[now->child->child->var_name].varsize = 4;
                    } else if (child1->type == TYPE_CHAR) {
                        nowtable->var[now->child->child->var_name] = VarNode(CHARR);
                        nowtable->var[now->child->child->var_name].pos = nowpos;
                        nowpos += 4;
                        if (now->child->sibling != nullptr)
                            nowtable->var[now->child->child->var_name].constval = now->child->sibling->ch_val;
                        nowtable->var[now->child->child->var_name].varsize = 4;
                    } else if (child1->type == TYPE_INT_CONST) {
                        if (now->child->sibling->exptype != INTEGER_VAL) {
                            cerror("can not init with not a integer");
                            cerror("can not init with not a integer");
                        }
                        nowtable->var[now->child->child->var_name] = VarNode(CONST_INT);
                        nowtable->var[now->child->child->var_name].pos = nowpos;
                        nowpos += 4;
                        nowtable->var[now->child->child->var_name].constval = now->child->sibling->int_val;
                        nowtable->var[now->child->child->var_name].varsize = 4;
                    } else if (child1->type == TYPE_CHAR_CONST) {
                        if (now->child->sibling->exptype != CHAR_VAL) {
                            cerror("can not init with not a char const");
                        }
                        nowtable->var[now->child->child->var_name] = VarNode(CONST_CHAR);
                        nowtable->var[now->child->child->var_name].pos = nowpos;
                        nowpos += 4;
                        nowtable->var[now->child->child->var_name].constval = now->child->sibling->ch_val;
                        nowtable->var[now->child->child->var_name].varsize = 4;
                    } else if (child1->type == TYPE_COMPOSE_STRUCT) {
                        if (typetableRoot->var.find(child1->var_name) == typetableRoot->var.end()) {
                            cerror("this struct not define");
                        }
                        if (typetableRoot->var[child1->var_name].basetype != STRUCT_DEF) {
                            cerror("this IDENTIFIER is not a struct");
                        }
                        nowtable->var[now->child->child->var_name] = VarNode(STRUCT);
                        nowtable->var[now->child->child->var_name].pos = nowpos;

                        nowtable->var[now->child->child->var_name].nametype = child1->var_name;
                        nowtable->var[now->child->child->var_name].varsize = typetableRoot->var[child1->var_name].varsize;
                        nowpos += nowtable->var[now->child->child->var_name].varsize;
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
//                        cout<<child1->var_name<<endl;
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
                    nowtable->var[now->child->child->var_name].pos = nowpos;
                    nowpos += base_array * basesize;
                } else {
                    cerror("not right type");
                }
                if (instruct)
                    nowtable->var[now->child->child->var_name].timestamp = -1;
                else
                    nowtable->var[now->child->child->var_name].timestamp = timestamp;
            }
            if (!instruct)
                timestamp++;
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
//            cout<<"hhh"<<endl;
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
            this->label = this->child->label;
            cout << this->label << ":" << endl;
            cout << "\t .string " << this->child->str_val << endl;
            if (temp != nullptr) {
                temp = temp->child;
                while (temp != nullptr) {
                    temp->getIdValType(nowtable);
                    temp = temp->sibling;
                }
            }
        } else if (this->stype == STMT_PRINT) {
            this->label = this->child->label;
            cout << this->label << ":" << endl;
            cout << "\t .string " << this->child->str_val << endl;
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
        nowpos = 4;
        if (nowtable->var.find(function_name) != nowtable->var.end()) {
            cerror("Repeated definition");
        }
        nowtable->var[function_name] = VarNode(FUNC);
        if (this->child->type == TYPE_INT) {
            nowtable->var[function_name].
                    returnType = INT;
        } else if (this->child->type == TYPE_CHAR) {
            nowtable->var[function_name].
                    returnType = CHARR;
        } else if (this->child->type == TYPE_VOID) {
            nowtable->var[function_name].
                    returnType = RETVOID;
        } else {
            cerror("function return these type not implement");
        }
        nowtable = nowtable->newChild();

//参数列表
        auto temp = this->child->sibling->sibling->child;
        int tmppos = -8;
        while (temp != nullptr) {
            auto para_name = temp->sibling->var_name;
            if (nowtable->var.find(para_name) != nowtable->var.end()) {
                cerror("Repeated definition");
            }
            if (temp->type == TYPE_INT) {
                nowtable->var[para_name] = VarNode(INT);
                nowtable->var[para_name].pos = tmppos;
            } else if (temp->type == TYPE_CHAR) {
                nowtable->var[para_name] = VarNode(CHARR);
                nowtable->var[para_name].pos = tmppos;
            } else {
                cerror("function parameter these type not implement");
            }
            tmppos -= 4;
            nowtable->var[para_name].timestamp = timestamp;
            nowtable->param_list.push_back(nowtable->var[para_name]);
            temp = temp->sibling;
            temp = temp->sibling;
        }
        timestamp++;
        nowtable->fa->structvar[function_name] =
                nowtable;

//函数体
        auto tmp = this->child->sibling->sibling->sibling->child;
        while (tmp != nullptr) {
            tmp->genTable(nowtable);
            tmp = tmp->sibling;
        }
        nowtable = nowtable->fa;
        nowtable->var[function_name].varsize = nowpos - 4;
        in_function = "";
    } else if (this->nodeType == NODE_STRUCT) {
        instruct = 1;
        auto struct_name = this->var_name;
        if (nowtable->var.find(struct_name) != nowtable->var.end()) {
            cerror("Repeated definition");
        }
        nowtable->var[struct_name] = VarNode(STRUCT_DEF);
        nowtable = nowtable->newChild();
        nowtable->fa->structvar[struct_name] =
                nowtable;
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
        instruct = 0;
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

VarNode TreeNode::findVar(namespore *nowtable, string name) {
    while (true) {
        if (nowtable == nullptr)
            break;
        if (nowtable->var.find(name) == nowtable->var.end() || nowtable->var[name].timestamp > timestamp)
            nowtable = nowtable->fa;
        else break;
    }
    if (nowtable == nullptr) {
        cerror("is nullptr??");
    }
    return nowtable->var[name];
}


void TreeNode::printAST(namespore *nowtable) {
    if (this->nodeType == NODE_PROG) {
        //声明全局变量
        timestamp = 0;
        auto temp = this->child->child;
        while (temp != nullptr) {
            if (temp->nodeType == NODE_STMT && temp->stype == STMT_DECL) {
                auto child2 = temp->child->sibling;//参数列表
                auto child1 = temp->child;
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
                }
            }
            temp = temp->sibling;
        }
        cout << ".section .data" << endl;

        for (auto &x:nowtable->var) {
            x.second.is_global = 1;
            if (x.second.basetype == INT || x.second.basetype == CONST_INT
                || x.second.basetype == CHARR || x.second.basetype == CONST_CHAR) {
                cout << x.first << ": .int " << x.second.constval << endl;
            } else if (x.second.basetype == INT_ARRAY || x.second.basetype == CHAR_ARRAY
                       || x.second.basetype == STRUCT || x.second.basetype == STRUCT_ARRAY) {
                cout << x.first << ": .fill " << x.second.varsize << endl;
            }
        }
        temp = this->child->child;
        while (temp != nullptr) {
            if (temp->nodeType == NODE_FUNC) {
                temp->printAST(typetableRoot->structvar[temp->child->sibling->var_name]);
            } else if (temp->nodeType == NODE_STMT) {
                timestamp++;
            } else if (temp->nodeType == NODE_STRUCT) {
//                temp->printAST(typetableRoot->structvar[temp->var_name]);
            }
            temp = temp->sibling;
        }

        //最后输出
        cout << ".section .note.GNU-stack,\"\",@progbits" << endl;
    } else if (this->nodeType == NODE_STMT) {
        if (in_function == "") {
            return;
        }
        if (this->stype == STMT_DECL) {
            timestamp++;
            auto temp = this->child->sibling->child;
            while (temp != nullptr) {
                if (temp->ftype == DEFINE_FORMAT_INIT) {
                    auto vattemp = findVar(nowtable, temp->child->var_name);
                    cout << "pushl %eax" << endl;
                    cout << "movl $" << temp->child->sibling->ch_val + temp->child->sibling->int_val << ", %eax"
                         << endl;
                    cout << "movl %eax," << -vattemp.pos << "(%ebp)" << endl;
                    cout << "popl %eax" << endl;
                }
                temp = temp->sibling;
            }
        } else if (this->stype == STMT_SCANF) {
            int back = 0;
            vector < TreeNode * > should_push;
            auto temp = this->child->sibling->child;
            while (temp != nullptr) {
                should_push.push_back(temp);
                temp = temp->sibling;
            }
            reverse(should_push.begin(), should_push.end());
            for (auto x:should_push) {
                x->printIdAdress(nowtable);
                back += 4;
            }
            cout << "pushl $" << this->label << endl;
            back += 4;
            cout << "call scanf" << endl;
            cout << "addl $" << back << ", %esp" << endl;
        } else if (this->stype == STMT_PRINT) {
            int back = 0;
            vector < TreeNode * > should_push;
            auto temp = this->child->sibling->child;
            while (temp != nullptr) {
                should_push.push_back(temp);
                temp = temp->sibling;
            }
            reverse(should_push.begin(), should_push.end());
            for (auto x:should_push) {
                x->printExpr(nowtable);
                back += 4;
            }
            cout << "pushl $" << this->label << endl;
            back += 4;
            cout << "call printf" << endl;
            cout << "addl $" << back << ", %esp" << endl;
        } else if (this->stype == STMT_FOR) {
            int hisnum = label_num;
            label_num++;

            loop_flag.push(make_pair("FOR_END" + to_string(hisnum), "FOR_INC" + to_string(hisnum)));

            this->child->printAST(nowtable);
            cout << "FOR_CHECK" << to_string(hisnum) << ":" << endl;
            this->child->sibling->printExpr(nowtable);
            cout << "popl %eax" << endl;
            cout << "and $1, %eax" << endl;
            cout << "cmp $1, %eax" << endl;
            cout << "je " << "FOR_WORK" << to_string(hisnum) << endl;
            cout << "jmp " << "FOR_END" << to_string(hisnum) << endl;
            cout << "FOR_WORK" << to_string(hisnum) << ":" << endl;
            //todo for循环stmt遍历
            auto tmp = this->child->sibling->sibling->sibling;
            if (tmp->stype == STMT_BLOCK) {
                tmp = tmp->child;
                auto temp = nowtable->child;
                while (tmp != nullptr) {
                    if (tmp->nodeType == NODE_STMT) {
                        if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                            tmp->stype == STMT_FOR) {
                            tmp->printAST(temp);
                            if (tmp->stype == STMT_IF_ELSE)
                                temp = temp->sibling;
                            temp = temp->sibling;
                        } else
                            tmp->printAST(nowtable);
                    } else tmp->printAST(nowtable);
                    tmp = tmp->sibling;
                }
            } else {
                auto temp = nowtable->child;
                if (tmp->nodeType == NODE_STMT) {
                    if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                        tmp->stype == STMT_FOR) {
                        tmp->printAST(temp);
                        if (tmp->stype == STMT_IF_ELSE)
                            temp = temp->sibling;
                        temp = temp->sibling;
                    } else
                        tmp->printAST(nowtable);
                    //todo function call
                } else tmp->printAST(nowtable);
            }
            cout << "FOR_INC" << to_string(hisnum) << ":" << endl;
            this->child->sibling->sibling->printAST(nowtable);
            cout << "jmp " << "FOR_CHECK" << to_string(hisnum) << endl;
            cout << "FOR_END" << to_string(hisnum) << ":" << endl;
            loop_flag.pop();

        } else if (this->stype == STMT_WHILE) {
            int hisnum = label_num;
            label_num++;
            loop_flag.push(make_pair("WHILE_END" + to_string(hisnum), "WHILE_CHECK" + to_string(hisnum)));
            cout << "WHILE_CHECK" << to_string(hisnum) << ":" << endl;
            this->child->printExpr(nowtable);
            cout << "popl %eax" << endl;
            cout << "and $1, %eax" << endl;
            cout << "cmp $1, %eax" << endl;
            cout << "je " << "WHILE_WORK" << to_string(hisnum) << endl;
            cout << "jmp " << "WHILE_END" << to_string(hisnum) << endl;
            cout << "WHILE_WORK" << to_string(hisnum) << ":" << endl;
            //todo while循环stmt遍历
            auto tmp = this->child->sibling;
            if (tmp->stype == STMT_BLOCK) {
                tmp = tmp->child;
                auto temp = nowtable->child;
                while (tmp != nullptr) {
                    if (tmp->nodeType == NODE_STMT) {
                        if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                            tmp->stype == STMT_FOR) {
                            tmp->printAST(temp);
                            if (tmp->stype == STMT_IF_ELSE)
                                temp = temp->sibling;
                            temp = temp->sibling;
                        } else
                            tmp->printAST(nowtable);
                    } else tmp->printAST(nowtable);
                    tmp = tmp->sibling;
                }
            } else {
                auto temp = nowtable->child;
                if (tmp->nodeType == NODE_STMT) {
                    if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                        tmp->stype == STMT_FOR) {
                        tmp->printAST(temp);
                        if (tmp->stype == STMT_IF_ELSE)
                            temp = temp->sibling;
                        temp = temp->sibling;
                    } else
                        tmp->printAST(nowtable);
                    //todo function call
                } else tmp->printAST(nowtable);
            }
            cout << "jmp " << "WHILE_CHECK" << to_string(hisnum) << endl;
            cout << "WHILE_END" << to_string(hisnum) << ":" << endl;
            loop_flag.pop();

        } else if (this->stype == STMT_IF) {
            int hisnum = label_num;
            label_num++;
            this->child->printExpr(nowtable);
            cout << "popl %eax" << endl;
            cout << "and $1, %eax" << endl;
            cout << "cmp $1, %eax" << endl;
            cout << "je " << "IF_WORK" << to_string(hisnum) << endl;
            cout << "jmp " << "IF_EXIT" << to_string(hisnum) << endl;
            cout << "IF_WORK" << to_string(hisnum) << ":" << endl;

            auto tmp = this->child->sibling;
            if (tmp->stype == STMT_BLOCK) {
                tmp = tmp->child;
                auto temp = nowtable->child;
                while (tmp != nullptr) {
                    if (tmp->nodeType == NODE_STMT) {
                        if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                            tmp->stype == STMT_FOR) {
                            tmp->printAST(temp);
                            if (tmp->stype == STMT_IF_ELSE)
                                temp = temp->sibling;
                            temp = temp->sibling;
                        } else
                            tmp->printAST(nowtable);
                    } else tmp->printAST(nowtable);
                    //todo function call
                    tmp = tmp->sibling;
                }
            } else {
                auto temp = nowtable->child;
                if (tmp->nodeType == NODE_STMT) {
                    if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                        tmp->stype == STMT_FOR) {
                        tmp->printAST(temp);
                        if (tmp->stype == STMT_IF_ELSE)
                            temp = temp->sibling;
                        temp = temp->sibling;
                    } else tmp->printAST(nowtable);
                } else tmp->printAST(nowtable);
            }
            cout << "IF_EXIT" << to_string(hisnum) << ":" << endl;

        } else if (this->stype == STMT_IF_ELSE) {
//            cout<<"hhhhhhhh"<<endl;
            int hisnum = label_num;
            label_num++;
            this->child->printExpr(nowtable);
            cout << "popl %eax" << endl;
            cout << "and $1, %eax" << endl;
            cout << "cmp $1, %eax" << endl;
            cout << "je " << "IF_THEN" << to_string(hisnum) << endl;
            cout << "jmp " << "IF_ELSE" << to_string(hisnum) << endl;
            cout << "IF_THEN" << to_string(hisnum) << ":" << endl;

            auto tmp = this->child->sibling;
            if (tmp->stype == STMT_BLOCK) {
                tmp = tmp->child;
                auto temp = nowtable->child;
                while (tmp != nullptr) {
                    if (tmp->nodeType == NODE_STMT) {
                        if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                            tmp->stype == STMT_FOR) {
                            tmp->printAST(temp);
                            if (tmp->stype == STMT_IF_ELSE)
                                temp = temp->sibling;
                            temp = temp->sibling;
                        } else
                            tmp->printAST(nowtable);
                    } else tmp->printAST(nowtable);
                    //todo function call
                    tmp = tmp->sibling;
                }
            } else {
                auto temp = nowtable->child;
                if (tmp->nodeType == NODE_STMT) {
                    if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                        tmp->stype == STMT_FOR) {
                        tmp->printAST(temp);
                        if (tmp->stype == STMT_IF_ELSE)
                            temp = temp->sibling;
                        temp = temp->sibling;
                    } else tmp->printAST(nowtable);
                } else tmp->printAST(nowtable);
            }
            cout << "jmp IF_EXIT" << to_string(hisnum) << endl;
            cout << "IF_ELSE" << to_string(hisnum) << ":" << endl;
            tmp = this->child->sibling->sibling;
            nowtable = nowtable->sibling;
            if (tmp->stype == STMT_BLOCK) {
                tmp = tmp->child;
                auto temp = nowtable->child;
                while (tmp != nullptr) {
                    if (tmp->nodeType == NODE_STMT) {
                        if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                            tmp->stype == STMT_FOR) {
                            tmp->printAST(temp);
                            if (tmp->stype == STMT_IF_ELSE)
                                temp = temp->sibling;
                            temp = temp->sibling;
                        } else
                            tmp->printAST(nowtable);
                    } else tmp->printAST(nowtable);
                    //todo function call
                    tmp = tmp->sibling;
                }
            } else {
                auto temp = nowtable->child;
                if (tmp->nodeType == NODE_STMT) {
                    if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                        tmp->stype == STMT_FOR) {
                        tmp->printAST(temp);
                        if (tmp->stype == STMT_IF_ELSE)
                            temp = temp->sibling;
                        temp = temp->sibling;
                    } else tmp->printAST(nowtable);
                } else tmp->printAST(nowtable);
            }
            cout << "IF_EXIT" << to_string(hisnum) << ":" << endl;
        } else if (this->stype == STMT_ASSIGN) {
            this->child->printIdAdress(nowtable);
            this->child->sibling->printExpr(nowtable);
            cout << "popl %eax" << endl;
            cout << "popl %ebx" << endl;
            cout << "movl %eax, (%ebx)" << endl;
        } else if (this->stype == STMT_ASSIGN_ADD) {
            this->child->printIdAdress(nowtable);
            this->child->sibling->printExpr(nowtable);
            cout << "popl %eax" << endl;
            cout << "popl %ebx" << endl;
            cout << "addl %eax, (%ebx)" << endl;
        } else if (this->stype == STMT_ASSIGN_MUL) {
            this->child->printIdAdress(nowtable);
            this->child->sibling->printExpr(nowtable);
            cout << "popl %ebx" << endl;
            cout << "popl %eax" << endl;
            cout << "movl %eax, %ecx" << endl;
            cout << "movl (%eax), %eax" << endl;
            cout << "imull %ebx" << endl;
            cout << "movl %eax, (%ecx)" << endl;
        } else if (this->stype == STMT_ASSIGN_SUB) {
            this->child->printIdAdress(nowtable);
            this->child->sibling->printExpr(nowtable);
            cout << "popl %eax" << endl;
            cout << "popl %ebx" << endl;
            cout << "subl %eax, (%ebx)" << endl;
        } else if (this->stype == STMT_ASSIGN_DIV) {
            this->child->printIdAdress(nowtable);
            this->child->sibling->printExpr(nowtable);
            cout << "popl %ebx" << endl;
            cout << "popl %eax" << endl;
            cout << "cltd" << endl;
            cout << "movl %eax, %ecx" << endl;
            cout << "movl (%eax), %eax" << endl;
            cout << "idivl %ebx" << endl;
            cout << "movl %eax, (%ecx)" << endl;
        } else if (this->stype == STMT_ASSIGN_MOD) {
            this->child->printIdAdress(nowtable);
            this->child->sibling->printExpr(nowtable);
            cout << "popl %ebx" << endl;
            cout << "popl %eax" << endl;
            cout << "cltd" << endl;
            cout << "movl %eax, %ecx" << endl;
            cout << "movl (%eax), %eax" << endl;
            cout << "idivl %ebx" << endl;
            cout << "movl %edx, (%ecx)" << endl;
        } else if (this->stype == STMT_ASSIGN_ADD_SELF) {
            this->child->printIdAdress(nowtable);
            cout << "popl %eax" << endl;
            cout << "addl $1, (%eax)" << endl;
        } else if (this->stype == STMT_BREAK) {
            auto tmp = loop_flag.top();
            cout << "jmp " << tmp.first << endl;
        } else if (this->stype == STMT_CONTINUE) {
            auto tmp = loop_flag.top();
            cout << "jmp " << tmp.second << endl;
        } else if (this->stype == STMT_RET) {
            if (this->child->nodeType != NODE_EMPTY) {
                this->child->printExpr(nowtable);
            }
            cout << "popl %eax" << endl;
            cout << "jmp " << in_function << "_EXIT" << endl;
        } else if (this->stype == STMT_FUNCTION_CALL) {
            this->printExpr(nowtable);
            cout << "popl %eax" << endl;
        }
    } else if (this->nodeType == NODE_FUNC) {
        cout << ".text" << endl;
        cout << ".globl " << this->var_name << endl;
        cout << ".type " << this->var_name << ", @function" << endl;
        cout << this->var_name << ":" << endl;
        cout << "pushl %ebp" << endl;
        cout << "movl %esp, %ebp" << endl;
        cout << "subl $" << typetableRoot->var[this->var_name].varsize << ",%esp" << endl;
        in_function = this->var_name;
        //参数列表偏移值计算 todo
        timestamp++;

        //函数体生成
        auto tmp = this->child->sibling->sibling->sibling->child;
        auto temp = nowtable->child;
        while (tmp != nullptr) {
            if (tmp->nodeType == NODE_STMT) {
                if (tmp->stype == STMT_IF || tmp->stype == STMT_IF_ELSE || tmp->stype == STMT_WHILE ||
                    tmp->stype == STMT_FOR) {
                    tmp->printAST(temp);
                    if (tmp->stype == STMT_IF_ELSE)
                        temp = temp->sibling;
                    temp = temp->sibling;
                } else
                    tmp->printAST(nowtable);
            } else tmp->printAST(nowtable);
            //todo function call
            tmp = tmp->sibling;
        }

        //todo return 修改
        cout << in_function << "_EXIT:" << endl;
        cout << "movl %ebp, %esp" << endl;
        cout << "popl %ebp" << endl;
        cout << "ret" << endl;
        in_function = "";

    } else if (this->nodeType == NODE_STRUCT) {
        auto temp = this->child->sibling->child;
        while (temp != nullptr) {
            if (temp->nodeType == NODE_STMT && temp->stype == STMT_DECL)
                timestamp++;
        }

    } else {
        cerror("not support asm in such");
    }
}

void TreeNode::printExpr(namespore *nowtable) {
    if ((this->nodeType == NODE_STMT && this->stype == STMT_FUNCTION_CALL)
        || (this->nodeType == NODE_EXPR && this->exptype == FUNC_CALL)) {
        auto tmp = this->child->sibling->child;
        vector < TreeNode * > should_put;
        while (tmp != nullptr) {
            should_put.push_back(tmp);
            tmp = tmp->sibling;
        }
        reverse(should_put.begin(), should_put.end());
        for (auto x:should_put) {
            x->printExpr(nowtable);
        }
        cout << "call " << this->var_name << endl;
        cout<<"add $"<<should_put.size()*4<<", %esp"<<endl;
        cout << "pushl %eax" << endl;
    } else if (this->exptype == IDENTIFIER_VAL) {
        this->printIdAdress(nowtable);
        cout << "popl %eax" << endl;
        cout << "pushl (%eax)" << endl;
    } else if (this->exptype == INTEGER_VAL) {
        cout << "pushl $" << this->int_val << endl;
    } else if (this->exptype == CHAR_VAL) {
        cout << "pushl $" << (int)this->ch_val << endl;
    } else if (this->exptype == ADD) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "addl %ebx, %eax" << endl;
        cout << "pushl %eax" << endl;
    } else if (this->exptype == SUB) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "subl %eax, %ebx" << endl;
        cout << "pushl %ebx" << endl;
    } else if (this->exptype == MUL) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "imull %ebx" << endl;
        cout << "pushl %eax" << endl;
    } else if (this->exptype == DIV) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %ebx" << endl;
        cout << "popl %eax" << endl;
        cout << "cltd" << endl;
        cout << "idivl %ebx" << endl;
        cout << "pushl %eax" << endl;
    } else if (this->exptype == MOD) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %ebx" << endl;
        cout << "popl %eax" << endl;
        cout << "cltd" << endl;
        cout << "idivl %ebx" << endl;
        cout << "pushl %edx" << endl;
    } else if (this->exptype == POS) {
        this->child->printExpr(nowtable);
    } else if (this->exptype == NEG) {
        this->child->printExpr(nowtable);
        cout << "movl $0, %ebx" << endl;
        cout << "popl %eax" << endl;
        cout << "subl %eax, %ebx" << endl;
        cout << "pushl %ebx" << endl;
    } else if (this->exptype == OR_BOOL) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "or %eax, %ebx" << endl;
        cout << "pushl %ebx" << endl;

    } else if (this->exptype == AND_BOOL) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "and %eax, %ebx" << endl;
        cout << "pushl %ebx" << endl;

    } else if (this->exptype == NOT_BOOL) {
        this->child->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "not %eax" << endl;
        cout << "pushl %eax" << endl;
    } else if (this->exptype == EQL) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "cmpl %eax, %ebx" << endl;
        cout << "sete %cl" << endl;
        cout << "pushl %ecx" << endl;
    } else if (this->exptype == SMALLEQL) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "cmpl %eax, %ebx" << endl;
        cout << "setle %cl" << endl;
        cout << "pushl %ecx" << endl;
    } else if (this->exptype == NOTEQL) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "cmpl %eax, %ebx" << endl;
        cout << "setne %cl" << endl;
        cout << "pushl %ecx" << endl;
    } else if (this->exptype == BIGEQL) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "cmpl %eax, %ebx" << endl;
        cout << "setge %cl" << endl;
        cout << "pushl %ecx" << endl;
    } else if (this->exptype == BIG) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "cmpl %eax, %ebx" << endl;
        cout << "setg %cl" << endl;
        cout << "pushl %ecx" << endl;
    } else if (this->exptype == SMALL) {
        this->child->printExpr(nowtable);
        this->child->sibling->printExpr(nowtable);
        cout << "popl %eax" << endl;
        cout << "popl %ebx" << endl;
        cout << "cmpl %eax, %ebx" << endl;
        cout << "setl %cl" << endl;
        cout << "pushl %ecx" << endl;
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
            auto rr= VarNode(temptable->var[id_name].basetype);
            rr.returnType=INT;
            return rr;
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
    if (this->nodeType == NODE_STMT) {
        auto tmp1 = this->child->getIdValType(nowtable);
//        cout<<this->child->var_name<<endl;
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

void TreeNode::printIdAdress(namespore *nowtable) {
    if (this->vartype == VAR_TYPE) {
        auto vattmp = findVar(nowtable, this->var_name);

        if (vattmp.is_global) {
            cout << "movl $" << this->var_name << ", %eax" << endl;
            cout << "pushl %eax" << endl;
        } else {
            cout << "movl %ebp, %eax" << endl;
            cout << "subl $" << vattmp.pos << ", %eax" << endl;
            cout << "pushl %eax" << endl;
        }
    } else if (this->vartype == ARRAY_TYPE) {
        auto vattmp = findVar(nowtable, this->var_name);
        auto temp = this->child->sibling->child;
        cout << "pushl $0" << endl;
        for (int i = 0; i < vattmp.dim_num.size() - 1; i++) {
            temp->printExpr(nowtable);
            cout << "popl %eax" << endl;
            int tmp = vattmp.dim_num[i];
            cout << "movl $" << tmp << ", %edx" << endl;
            cout << "imull %edx" << endl;
            cout << "popl %ebx" << endl;
            cout << "addl %ebx, %eax" << endl;
            cout << "pushl %eax" << endl;
            temp = temp->sibling;
        }
        temp->printExpr(nowtable);
        cout << "popl %ebx" << endl;
        cout << "popl %eax" << endl;
        cout << "addl %ebx, %eax" << endl;
        if (vattmp.is_global) {
            cout << "movl $" << this->var_name << ", %ebx" << endl;
            cout << "movl $4, %edx" << endl;
            cout << "imull %edx" << endl;
            cout << "addl %ebx, %eax" << endl;
            cout << "pushl %eax" << endl;
        } else {
            cout << "movl $4, %edx" << endl;
            cout << "imull %edx" << endl;
            cout << "subl %ebp, %eax" << endl;
            cout << "movl $0, %ebx" << endl;
            cout << "subl $" << vattmp.pos << ", %ebx" << endl;
            cout << "subl %eax, %ebx" << endl;
            cout << "pushl %ebx" << endl;
        }
    } else if (this->vartype == STRUCT_TYPE) {

        auto vartemp = findVar(nowtable, this->var_name);
        int thisis_g = vartemp.is_global;
        if (this->child->vartype == VAR_TYPE) {
            if (vartemp.is_global) {
                cout << "movl $" << this->var_name << ", %eax" << endl;
                cout << "pushl %eax" << endl;
            } else {
                cout << "movl %ebp, %eax" << endl;
                cout << "subl $" << vartemp.pos << ", %eax" << endl;
                cout << "pushl %eax" << endl;
            }
        } else {
            cout << "pushl $0" << endl;
            auto temp = this->child->child->sibling->child;
            for (int i = 0; i < vartemp.dim_num.size() - 1; i++) {
                temp->printExpr(nowtable);
                cout << "popl %eax" << endl;
                int tmp = vartemp.dim_num[i];
                cout << "movl $" << tmp << ", %edx" << endl;
                cout << "imull %edx" << endl;
                cout << "popl %ebx" << endl;
                cout << "addl %ebx, %eax" << endl;
                cout << "pushl %eax" << endl;
                temp = temp->sibling;
            }
            temp->printExpr(nowtable);
            cout << "popl %ebx" << endl;
            cout << "popl %eax" << endl;
            cout << "addl %ebx, %eax" << endl;

            if (vartemp.is_global) {
                cout << "movl $" << this->var_name << ", %ebx" << endl;
                cout << "movl $" << typetableRoot->var[vartemp.nametype].varsize << ", %edx" << endl;
                cout << "imull %edx" << endl;
                cout << "addl %ebx, %eax" << endl;
                cout << "pushl %eax" << endl;
            } else {
                cout << "movl $" << typetableRoot->var[vartemp.nametype].varsize << ", %edx" << endl;
                cout << "imull %edx" << endl;
                cout << "subl %ebp, %eax" << endl;
                cout << "movl $0, %ebx" << endl;
                cout << "subl $" << vartemp.pos << ", %ebx" << endl;
                cout << "subl %eax, %ebx" << endl;
                cout << "pushl %ebx" << endl;
            }
        }

        auto structtable = typetableRoot->structvar[vartemp.nametype];
        vartemp = findVar(structtable, this->child->sibling->var_name);
        if (this->child->sibling->vartype == VAR_TYPE) {
            cout << "movl $" << vartemp.offset_struct << ", %eax" << endl;
            cout << "pushl %eax" << endl;
        } else {
            cout << "pushl $0" << endl;
            auto temp = this->child->sibling->child->sibling->child;
            for (int i = 0; i < vartemp.dim_num.size() - 1; i++) {
                temp->printExpr(nowtable);
                cout << "popl %eax" << endl;
                int tmp = vartemp.dim_num[i];
                cout << "movl $" << tmp << ", %edx" << endl;
                cout << "imull %edx" << endl;
                cout << "popl %ebx" << endl;
                cout << "addl %ebx, %eax" << endl;
                cout << "pushl %eax" << endl;
                temp = temp->sibling;
            }
            temp->printExpr(nowtable);
            cout << "popl %ebx" << endl;
            cout << "popl %eax" << endl;
            cout << "addl %ebx, %eax" << endl;
            cout << "movl $4, %edx" << endl;
            cout << "imull %edx" << endl;
            cout << "pushl %eax" << endl;
        }

        cout << "popl %ebx" << endl;
        cout << "popl %eax" << endl;
        if (thisis_g) {
            cout << "addl %ebx, %eax" << endl;
        } else {
            cout << "subl %ebx, %eax" << endl;
        }
        cout << "pushl %eax" << endl;
    }


}

VarNode::VarNode(Basetype a = INT) {
    this->basetype = a;
}

void TreeNode::cerror(const char *mess) {
    cout << mess << endl;
    exit(1);
}