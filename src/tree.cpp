#include "tree.h"

int VarNode::nodeID = 0;
int TreeNode::nowID = 0;

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

void VarNode::addChild(VarNode *child) {
    if (this->child == nullptr) {
        this->child = child;
    } else {
        VarNode *tmp = this->child;
        while (tmp->sibling != nullptr) {
            tmp = tmp->sibling;
        }
        tmp->sibling = child;
    }
    child->fa = this;
}

void TreeNode::genNodeId() {
    TreeNode *tmp = this;
    while (tmp != nullptr) {
        tmp->nodeID = TreeNode::nowID++;
        if (tmp->child != nullptr)
            tmp->child->genNodeId();
        tmp = tmp->sibling;
    }
}

void TreeNode::printNodeInfo() {
    //ID:xx  statement  childID:xx xx xx   VarID:xx
    printf("ID:%d ", this->nodeID);
    this->printSpecialInfo();
    this->printChildrenId();
    if (this->nodeType == NODE_VAR)
        printf("\t\tVarID: %d->%s", this->var_id, this->var_name.c_str());
    printf("\n");
}

void TreeNode::printChildrenId() {
    TreeNode *tmp = this->child;
    printf("\t\tChildID: ");
    while (tmp != nullptr) {
        printf("%d ", tmp->nodeID);
        tmp = tmp->sibling;
    }
}

void TreeNode::printAST() {
    TreeNode *tmp = this;
    while (tmp != nullptr) {
        tmp->printNodeInfo();
        if (tmp->child != nullptr)
            tmp->child->printAST();
        tmp = tmp->sibling;
    }
}


// You can output more info...
void TreeNode::printSpecialInfo() {
    switch (this->nodeType) {
        case NODE_CONST:
            if (this->type->type == VALUE_CHAR)
                printf("\t\tconst: char->\'%c\'", this->ch_val);
            else if (this->type->type == VALUE_INT)
                printf("\t\tconst: int->%d ", this->int_val);
            else if (this->type->type == VALUE_BOOL)
                printf("\t\tconst: bool->%s", this->b_val ? "true" : "false");
            else if (this->type->type == VALUE_STRING)
                printf("\t\tconst: string->\"%s\"", this->str_val.c_str());
            break;
        case NODE_VAR:
            printf("\t\tvar: %s", this->var_name.c_str());
            break;
        case NODE_EXPR:
            printf("\t\texpression");
            break;
        case NODE_STMT:
            if (this->stype == STMT_SKIP)
                printf("\t\tstatement: ;");
            else if (this->stype == STMT_DECL)
                printf("\t\tstatement: var_define");
            else if (this->stype == STMT_ASSIGN)
                printf("\t\tstatement: var_assign");
            break;
        case NODE_TYPE:
            if (this->type->type == VALUE_CHAR)
                printf("\t\ttype: char");
            else if (this->type->type == VALUE_INT)
                printf("\t\ttype: int");
            break;
        default:
            break;
    }
}

//string TreeNode::sType2String(StmtType type) {
//    return "?";
//}
//
//
//string TreeNode::nodeType2String(NodeType type) {
//    return "<>";
//}
