#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"
#include "analyzer.h"
#include "tac.h"

using namespace std;

void printBreak() {
    cout << string(60, '-') << "\n";
}

int main()
{
    ifstream file("test_code/sample.c");
    stringstream buffer;
    buffer << file.rdbuf();
    string code = buffer.str();

    auto tokens = tokenize(code);
    
    printBreak();
    cout << "\n"; printBreak();
    cout << "  LEXICAL ANALYSIS TOKENS\n"; printBreak();
    cout << left << "| " << setw(20) << "TOKEN TYPE" << "| " << setw(25) << "VALUE" << "| " << "LINE |\n";
    printBreak();
    for(auto t : tokens) {
         cout << left << "| " << setw(20) << t.type << "| " << setw(25) << t.value << "| " << t.line << "    |\n";
    }
    printBreak();

    cout << "\n"; printBreak();
    cout << "  ABSTRACT SYNTAX TREE (AST)\n"; printBreak();
    ASTNode* root = parse(tokens);
   string astOutput = "";
    printAST(root, 0, astOutput);
    cout << astOutput;
    printBreak();

    if (parserError != "") {
        cout << "\n"; printBreak();
        cout << "  SYNTAX ERROR\n"; printBreak();
        cout << "Syntax Error (Line " << errorLine << "): " << parserError << "\n";
        cout << "|||ERROR_LINE:" << errorLine << "|||\n"; // Signals UI to highlight
        printBreak();
    } else {
        cout << "\n"; printBreak();
        cout << "  SYNTAX CHECK\n"; printBreak();
        cout << "Syntax OK\n";
        printBreak();
        
        cout << "\n"; printBreak();
        cout << "  THREE ADDRESS CODE (TAC)\n"; printBreak();
        auto tac = generateTAC(root);
        if(tac.empty()) cout << "(No TAC generated for flat structure)" << endl;
        for(auto t : tac) cout << "  " << t << endl;
        printBreak();
    }

    cout << "\n"; printBreak();
    cout << "  SYMBOL TABLE\n"; printBreak();
    auto table = buildSymbolTable(code);
    if(table.empty()) {
        cout << "| (Empty)                                                  |\n";
    } else {
        cout << left << "| " << setw(20) << "DATA TYPE" << "| " << "VARIABLE NAME                |\n";
        printBreak();
        for(auto s : table) {
             cout << left << "| " << setw(20) << s.type << "| " << setw(29) << s.name << "|\n";
        }
    }
    printBreak();

    cout << "\n"; printBreak();
    cout << "  STATIC CODE ANALYSIS\n"; printBreak();
    bool hasErrorSignal = (parserError != "");
    auto undeclared = findUndeclared(tokens, table);
    for(auto u : undeclared) {
        cout << "[ERROR] Semantic Error (Line " << u.line << "): " << u.message << endl;
        if (!hasErrorSignal) {
            cout << "|||ERROR_LINE:" << u.line << "|||\n";
            hasErrorSignal = true; // Output only one error line signal to jump to first semantic error
        }
    }
    auto unused = findUnused(tokens,table);
    for(auto u : unused) cout << "[WARNING] Unused Variable: " << u << endl;

    auto issues = securityCheck(tokens);
    for(auto i : issues) cout << "[SECURITY] " << i << endl;

    cout << "[METRIC] Cyclomatic Complexity: " << cyclomaticComplexity(tokens) << endl;
    printBreak();
    
    return 0;
}