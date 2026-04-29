#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <vector>
#include <string>

using namespace std;

struct Symbol
{
    string type;
    string name;
};

vector<Symbol> buildSymbolTable(string code);

#endif