#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>

using namespace std;

struct Token {
    string type;
    string value;
    int line;
};

vector<Token> tokenize(const string& code);
vector<string> lexicalAnalysis(string code);

#endif