#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "lexer.h"

using namespace std;

struct ASTNode {
    string type;
    string value;
    vector<ASTNode*> children;
    ASTNode(string t, string v = "") : type(t), value(v) {}
};

extern string parserError;
extern int errorLine;

ASTNode* parse(vector<Token>& tokens);
string syntaxCheck(string code); 
void printAST(ASTNode* node, int depth, string& out);

#endif