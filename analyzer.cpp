#include "analyzer.h"
#include <set>

vector<SemanticIssue> findUndeclared(const vector<Token>& tokens, const vector<Symbol>& table)
{
    vector<SemanticIssue> issues;
    set<string> declaredVars;
    for(auto s : table) {
        declaredVars.insert(s.name);
    }
    
    set<string> builtins = {"include", "iostream", "using", "namespace", "std", "main", "cout", "cin", "endl", "printf", "scanf", "string", "vector", "stdio", "h", "math", "stdlib", "time"};

    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == "IDENTIFIER") {
            string name = tokens[i].value;
            
            // Ignore preprocessor directives like #include
            bool isDirective = false;
            for (int j = (int)i - 1; j >= 0; j--) {
                if (tokens[j].line != tokens[i].line) break;
                if (tokens[j].value == "#") {
                    isDirective = true;
                    break;
                }
            }
            if (isDirective) continue;

            if (builtins.count(name)) continue;
            
            if (i + 1 < tokens.size() && tokens[i+1].value == "(") continue;
            
            if (i > 0 && tokens[i-1].type == "KEYWORD" && 
               (tokens[i-1].value == "int" || tokens[i-1].value == "float" || tokens[i-1].value == "double" || tokens[i-1].value == "char")) continue;
            
            if (!declaredVars.count(name)) {
                bool found = false;
                for (auto iss : issues) {
                    if (iss.message.find("'" + name + "'") != string::npos) found = true;
                }
                if (!found) {
                    issues.push_back({"Undeclared variable '" + name + "'", tokens[i].line});
                }
            }
        }
    }
    return issues;
}

// Scans individual explicitly defined IDENTIFIER tokens rather than raw strings to prevent substring false-positives
vector<string> findUnused(const vector<Token>& tokens, vector<Symbol> table)
{
    vector<string> unused;

    for(auto s : table)
    {
        int count = 0;
        
        for(auto t : tokens) {
            if (t.type == "IDENTIFIER" && t.value == s.name) {
                count++;
            }
        }

        // If the variable only appears once in the entire token stream, it is only declared but never actually used.
        if (count <= 1)
            unused.push_back(s.name);
    }

    return unused;
}

// Scans only functional IDENTIFIERS, preventing false positives inside comments, variables, or strings
vector<string> securityCheck(const vector<Token>& tokens)
{
    vector<string> issues;

    for(auto t : tokens) {
        if (t.type == "IDENTIFIER") {
            if (t.value == "gets")
                issues.push_back("Unsafe function: gets()");
            if (t.value == "strcpy")
                issues.push_back("Unsafe function: strcpy()");
        }
    }

    return issues;
}

// Computes graph complexity pathing precisely based on control flow KEYWORDS and Logicals
int cyclomaticComplexity(const vector<Token>& tokens)
{
    int complexity = 1;

    for(auto t : tokens) {
        if (t.type == "KEYWORD") {
            if (t.value == "if" || t.value == "for" || t.value == "while") {
                complexity++;
            }
        } else if (t.type == "OPERATOR") {
            if (t.value == "&&" || t.value == "||") {
                complexity++;
            }
        }
    }

    return complexity;
}