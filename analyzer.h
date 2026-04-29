#ifndef ANALYZER_H
#define ANALYZER_H

#include <vector>
#include <string>
#include "symbol_table.h"
#include "lexer.h"

using namespace std;

struct SemanticIssue {
    string message;
    int line;
};

vector<SemanticIssue> findUndeclared(const vector<Token>& tokens, const vector<Symbol>& table);

vector<string> findUnused(const vector<Token>& tokens, vector<Symbol> table);

vector<string> securityCheck(const vector<Token>& tokens);

int cyclomaticComplexity(const vector<Token>& tokens);

#endif