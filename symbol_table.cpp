#include "symbol_table.h"
#include "lexer.h"

// Replaces previously naive string mapping with token-based logic
vector<Symbol> buildSymbolTable(string code)
{
    vector<Symbol> table;
    auto tokens = tokenize(code);
    string prevType = "";

    for (size_t i = 0; i < tokens.size(); i++) {
        auto& t = tokens[i];
        if (t.type == "KEYWORD" && (t.value == "int" || t.value == "float")) {
            prevType = t.value;
        } else if (t.type == "IDENTIFIER" && prevType != "") {
            // If the next token is a parenthesis '(', it's a function (like main), not a variable!
            bool isFunction = false;
            if (i + 1 < tokens.size() && tokens[i+1].value == "(") {
                isFunction = true;
            }

            if (!isFunction) {
                Symbol s;
                s.type = prevType;
                s.name = t.value;
                table.push_back(s);
            }
            // Reset after assigning type
            prevType = ""; 
        } else if (t.type == "PUNCTUATION" && t.value == ",") {
            // keep the same type if multiple variables on same line? basic logic omits
        } else {
            prevType = "";
        }
    }
    return table;
}