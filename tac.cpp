#include "tac.h"

int tempCounter = 1;
vector<string> tacOut;

string traverseTAC(ASTNode* node) {
    if (!node) return "";
    if (node->type == "IDENTIFIER" || node->type == "NUMBER") return node->value;
    
    if (node->type == "BIN_OP") {
        string left = traverseTAC(node->children[0]);
        string right = traverseTAC(node->children[1]);
        string t = "t" + to_string(tempCounter++);
        tacOut.push_back(t + " = " + left + " " + node->value + " " + right);
        return t;
    }

    if (node->type == "ASSIGNMENT") {
        string right = traverseTAC(node->children[1]);
        string left = node->children[0]->value;
        tacOut.push_back(left + " = " + right);
        return left;
    }

    if (node->type == "DECLARATION") {
        if (node->children.size() > 1) { // has assignment
            traverseTAC(node->children[1]);
        }
        return "";
    }

    // PROGRAM or block
    for(auto child : node->children) {
        traverseTAC(child);
    }
    return "";
}

vector<string> generateTAC(ASTNode* root) {
    tempCounter = 1;
    tacOut.clear();
    traverseTAC(root);
    return tacOut;
}
