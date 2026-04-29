#include "lexer.h"
#include <iostream>
#include <set>
#include <cctype>

set<string> keywords = {"int", "float", "if", "else", "while", "return", "printf"};

vector<Token> tokenize(const string& code) {
    vector<Token> tokens;
    int line = 1;
    for (size_t i = 0; i < code.length(); ) {
        if (isspace(code[i])) {
            if (code[i] == '\n') line++;
            i++;
            continue;
        }

        // Ignore single-line comments //
        if (code[i] == '/' && i + 1 < code.length() && code[i+1] == '/') {
            while (i < code.length() && code[i] != '\n') i++;
            continue;
        }

        // Ignore multi-line comments /* */
        if (code[i] == '/' && i + 1 < code.length() && code[i+1] == '*') {
            i += 2;
            while (i + 1 < code.length() && !(code[i] == '*' && code[i+1] == '/')) {
                if (code[i] == '\n') line++;
                i++;
            }
            i += 2;
            continue;
        }

        if (isalpha(code[i]) || code[i] == '_') {
            string word = "";
            while (i < code.length() && (isalnum(code[i]) || code[i] == '_')) {
                word += code[i++];
            }
            if (keywords.count(word)) tokens.push_back({"KEYWORD", word, line});
            else tokens.push_back({"IDENTIFIER", word, line});
        } 
        else if (isdigit(code[i])) {
            string num = "";
            while (i < code.length() && (isdigit(code[i]) || code[i] == '.')) {
                num += code[i++];
            }
            tokens.push_back({"NUMBER", num, line});
        } 
        else if (code[i] == '"') {
            string str = "";
            i++; // Skip opening quote
            while (i < code.length() && code[i] != '"') {
                if (code[i] == '\\' && i + 1 < code.length()) {
                    str += code[i++]; // Include backslash
                }
                str += code[i++];
            }
            if (i < code.length()) i++; // Skip closing quote
            tokens.push_back({"STRING_LITERAL", str, line});
        }
        else {
            string op(1, code[i]);
            if (code[i] == '=' || code[i] == '+' || code[i] == '-' || code[i] == '*' || code[i] == '/' ||
                code[i] == '<' || code[i] == '>' || code[i] == '!') {
                if (i + 1 < code.length() && code[i+1] == '=') {
                    op += code[++i];
                }
                tokens.push_back({"OPERATOR", op, line});
            } else {
                tokens.push_back({"PUNCTUATION", op, line});
            }
            i++;
        }
    }
    return tokens;
}

vector<string> lexicalAnalysis(string code) {
    auto tokens = tokenize(code);
    vector<string> res;
    for (auto t : tokens) {
        res.push_back(t.type + ": " + t.value + " (Line " + to_string(t.line) + ")");
    }
    return res;
}