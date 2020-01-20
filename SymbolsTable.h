//
// Created by szymon on 28.12.2019.
//

#ifndef LATC_X86_64_SYMBOLSTABLE_H
#define LATC_X86_64_SYMBOLSTABLE_H


#include <stack>
#include <map>
#include <set>
#include "Absyn.H"
#include "Error.h"

class VarInfo{
public:
    String ident;
    String type;
    int depth;

    VarInfo(const String &ident, const String &type, int depth);
};

class SymbolsTable {

    int lastSymbol = 0;
    int depth = 0;
    std::map<String, std::stack< VarInfo > > symbolsMap;
    std::map<String, String> allSymbolsMap;
    std::set<String> symbolsSet;
public:
    SymbolsTable();

    SymbolsTable(int lastSymbol, int depth, const std::map<String, std::stack<VarInfo>> &symbolsMap,
                 const std::set<String> &symbolsSet);

    void addSymbol(Ident ident, String type, int lineNumber, Error &error);
    void addSymbol(Ident ident1, Ident ident2, String type);
    void addLabel(Ident ident);
    String getSymbol(Ident ident1);
    String getType(Ident ident, int lineNumber, Error &error);
    void entryScope();
    void exitScope();
    String getNewSymbol();
    String getNewSymbol(Ident ident);
};




#endif //LATC_X86_64_SYMBOLSTABLE_H
