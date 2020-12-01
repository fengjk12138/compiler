#include "tree.h"

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
    this->sibling = sibling;
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
        tmp->nodeID = nowID++;
        tmp->child->genNodeId();
        tmp = tmp->sibling;
    }
}

void TreeNode::printNodeInfo() {

}

void TreeNode::printChildrenId() {

}

void TreeNode::printAST() {

}


// You can output more info...
void TreeNode::printSpecialInfo() {
    switch (this->nodeType) {
        case NODE_CONST:
            break;
        case NODE_VAR:
            break;
        case NODE_EXPR:
            break;
        case NODE_STMT:
            break;
        case NODE_TYPE:
            break;
        default:
            break;
    }
}

string TreeNode::sType2String(StmtType type) {
    return "?";
}


string TreeNode::nodeType2String(NodeType type) {
    return "<>";
}
