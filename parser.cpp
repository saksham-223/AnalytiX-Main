#include "parser.h"
#include <stack>
#include <iostream>

string parserError = "";
int errorLine = -1;

class Parser {
    vector<Token> tokens;
    size_t pos = 0;

public:
    Parser(vector<Token> t) : tokens(t) {}

    Token current() {
        if (pos < tokens.size()) return tokens[pos];
        return {"EOF", "", -1};
    }

    void advance() { pos++; }

    ASTNode* parseExpression() {
        Token t = current();
        if (t.type == "IDENTIFIER" || t.type == "NUMBER" || (t.type == "OPERATOR" && t.value == "-")) {
            bool isNeg = false;
            if (t.value == "-") {
                isNeg = true;
                advance();
                t = current();
                if (t.type != "NUMBER" && t.type != "IDENTIFIER") return nullptr;
            }
            
            advance();
            ASTNode* node = new ASTNode(t.type, (isNeg?"-":"") + t.value);
            
            Token op = current();
            if (op.type == "OPERATOR" && op.value != "=" && op.value != ";" ) {
                advance();
                ASTNode* binOp = new ASTNode("BIN_OP", op.value);
                binOp->children.push_back(node);
                ASTNode* right = parseExpression();
                if (right) {
                    binOp->children.push_back(right);
                    return binOp;
                }
            }
            return node;
        }
        return nullptr;
    }

    ASTNode* parseStatement() {
        Token t = current();
        if (t.type == "EOF") return nullptr;

        // Gracefully ignore structural C scopes for flat-AST assignment testing
        if (t.type == "PUNCTUATION" && (t.value == "{" || t.value == "}" || t.value == "(" || t.value == ")" || t.value == ",")) {
            advance();
            return nullptr;
        }
        
        // Ignore Preprocessor Directives like #include
        if (t.type == "PUNCTUATION" && t.value == "#") {
            while (pos < tokens.size() && current().line == t.line) advance();
            return nullptr;
        }
        
        // Ignore C++ specifics and IO streams (using namespace, cout, cin, printf)
        if ((t.type == "IDENTIFIER" || t.type == "KEYWORD") && (t.value == "using" || t.value == "cout" || t.value == "cin" || t.value == "printf" || t.value == "scanf" || t.value == "std")) {
            while (pos < tokens.size() && current().value != ";") advance();
            if (current().value == ";") advance();
            return nullptr;
        }

        if (t.type == "KEYWORD" && t.value == "return") {
            advance();
            parseExpression(); // Consume return value
            if (current().value == ";") advance();
            return nullptr;
        }

        if (t.type == "KEYWORD" && t.value == "if") {
            advance();
            // Skip condition ( ... )
            int parens = 0;
            while (pos < tokens.size()) {
                if (current().value == "(") parens++;
                if (current().value == ")") parens--;
                advance();
                if (parens == 0) break;
            }
            // Skip block { ... } or single statement
            if (current().value == "{") {
                int braces = 0;
                while (pos < tokens.size()) {
                    if (current().value == "{") braces++;
                    if (current().value == "}") braces--;
                    advance();
                    if (braces == 0) break;
                }
            } else {
                parseStatement();
            }
            return nullptr;
        }

        if (t.type == "KEYWORD" && (t.value == "int" || t.value == "float" || t.value == "char")) {
             string typeVar = t.value;
             advance();
             Token id = current();
             if (id.type == "IDENTIFIER") {
                 advance();
                 
                 if (current().value == "(") return nullptr; // Ignore function declarations

                 ASTNode* decl = new ASTNode("DECLARATION", typeVar);
                 decl->children.push_back(new ASTNode("IDENTIFIER", id.value));

                 if (current().value == "=") {
                     advance();
                     ASTNode* assign = new ASTNode("ASSIGNMENT", "=");
                     assign->children.push_back(new ASTNode("IDENTIFIER", id.value));
                     ASTNode* expr = parseExpression();
                     if (expr) {
                         assign->children.push_back(expr);
                     } else {
                         parserError = "Expected expression after '='";
                         errorLine = current().line;
                         return nullptr;
                     }
                     decl->children.push_back(assign);
                 }
                 
                 if (current().value == ";") {
                     advance();
                 } else {
                     parserError = "Missing ';' after declaration";
                     errorLine = current().line;
                     return nullptr;
                 }
                 return decl;
             } else {
                 parserError = "Expected identifier after '" + typeVar + "'";
                 errorLine = id.line;
                 return nullptr;
             }
        }

        if (t.type == "IDENTIFIER") {
            advance();
            Token nxt = current();
            if (nxt.value == "=" || nxt.value == "+=" || nxt.value == "-=") {
                advance();
                ASTNode* assign = new ASTNode("ASSIGNMENT", nxt.value);
                assign->children.push_back(new ASTNode("IDENTIFIER", t.value));
                ASTNode* expr = parseExpression();
                if (expr) {
                    assign->children.push_back(expr);
                } else {
                    parserError = "Expected expression after '" + nxt.value + "'";
                    errorLine = nxt.line;
                    return nullptr;
                }
                if (current().value == ";") {
                    advance();
                } else {
                    parserError = "Missing ';' after assignment";
                    errorLine = current().line;
                    return nullptr;
                }
                return assign;
            }
            pos--; // Undo advance
        }

        // Catch-all strict syntax enforcer:
        parserError = "Unrecognized statement or unexpected token: '" + current().value + "'";
        errorLine = current().line;
        advance(); 
        return nullptr;
    }

    ASTNode* parseProgram() {
        ASTNode* root = new ASTNode("PROGRAM");
        while (pos < tokens.size()) {
            if (current().type == "EOF") break;
            ASTNode* stmt = parseStatement();
            if (stmt) {
                root->children.push_back(stmt);
            }
        }
        return root;
    }
};

ASTNode* parse(vector<Token>& tokens) {
    parserError = "";
    errorLine = -1;
    Parser p(tokens);
    return p.parseProgram();
}

string syntaxCheck(string code) {
    auto tokens = tokenize(code);
    ASTNode* root = parse(tokens);
    if (parserError != "") {
        return "Syntax Error (Line " + to_string(errorLine) + "): " + parserError;
    }
    return "Syntax OK";
}

static void printASTHelper(ASTNode* node, string prefix, bool isLast, bool isRoot, string& out) {
    if (!node) return;
    
    if (!isRoot) {
        out += prefix;
        out += (isLast ? "└── " : "├── ");
    }
    
    out += node->type;
    if (node->value != "") out += " (" + node->value + ")";
    out += "\n";

    for (size_t i = 0; i < node->children.size(); ++i) {
        bool childIsLast = (i == node->children.size() - 1);
        string newPrefix = prefix;
        if (!isRoot) {
            newPrefix += (isLast ? "    " : "│   ");
        }
        printASTHelper(node->children[i], newPrefix, childIsLast, false, out);
    }
}

void printAST(ASTNode* node, int depth, string& out) {
    printASTHelper(node, "", true, true, out);
}