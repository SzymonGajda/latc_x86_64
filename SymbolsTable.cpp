//
// Created by szymon on 28.12.2019.
//

#include "SymbolsTable.h"

SymbolsTable::SymbolsTable() {}

void SymbolsTable::addSymbol(Ident ident, String type, int lineNumber, Error &error) {
    if (symbolsMap.find(ident) == symbolsMap.end() || symbolsMap.find(ident)->second.empty()) {
        std::stack<VarInfo> stack;
        stack.push(VarInfo("", type, depth));
        symbolsMap[ident] = stack;
        return;
    }
    auto temp = symbolsMap.find(ident)->second;
    if (temp.top().depth == depth) {
        error.setIsError(true);
        error.setErrorMessage("Error at line " + std::to_string(lineNumber) +
                              ": variable " + ident + " is already declared in this scope\n");
        return;
    }
    symbolsSet.insert(ident);
    temp.push(VarInfo("", type, depth));
    symbolsMap[ident] = temp;
}

String SymbolsTable::getType(Ident ident, int lineNumber, Error &error) {
    if (symbolsMap.find(ident) == symbolsMap.end()) {
        error.setIsError(true);
        error.setErrorMessage("Error at line " + std::to_string(lineNumber) +
                              ": variable " + ident + " is not declared\n");
        return "";
    }
    return symbolsMap.find(ident)->second.top().type;
}

void SymbolsTable::entryScope() {
    depth++;
}

void SymbolsTable::exitScope() {
    for (auto const &p : symbolsMap) {
        if (!p.second.empty() && p.second.top().depth == depth) {
            auto temp = p.second;
            temp.pop();
            symbolsMap[p.first] = temp;
        }
    }
    depth--;
}

String SymbolsTable::getNewSymbol() {
    while (symbolsSet.find("L" + std::to_string(lastSymbol)) != symbolsSet.end()) {
        lastSymbol++;
    }
    symbolsSet.insert("L" + std::to_string(lastSymbol));
    return "L" + std::to_string(lastSymbol);
}

void SymbolsTable::addSymbol(Ident ident1, Ident ident2, String type) {
    if (symbolsMap.find(ident1) == symbolsMap.end() || symbolsMap.find(ident1)->second.empty()) {
        std::stack<VarInfo> stack;
        stack.push(VarInfo(ident2, type, depth));
        symbolsMap[ident1] = stack;
        symbolsSet.insert(ident1);
        symbolsSet.insert(ident2);
        return;
    }
    auto temp = symbolsMap.find(ident1)->second;
    symbolsSet.insert(ident1);
    symbolsSet.insert(ident2);
    temp.push(VarInfo(ident2, type, depth));
    symbolsMap[ident1] = temp;
    allSymbolsMap.emplace(ident1, ident2);
}

String SymbolsTable::getSymbol(Ident ident1) {
    return symbolsMap.find(ident1)->second.top().ident;
}

void SymbolsTable::addLabel(Ident ident) {
    symbolsSet.insert(ident);
}

String SymbolsTable::getNewSymbol(Ident ident) {
    Ident newIdent = getNewSymbol();
    VarInfo varInfo = symbolsMap.find(ident)->second.top();
    symbolsMap.find(ident)->second.pop();
    varInfo.ident = newIdent;
    symbolsMap.find(ident)->second.push(varInfo);
    allSymbolsMap.emplace(ident, newIdent);
    return newIdent;
}

SymbolsTable::SymbolsTable(int lastSymbol, int depth, const std::map<String, std::stack<VarInfo>> &symbolsMap,
                           const std::set<String> &symbolsSet) : lastSymbol(lastSymbol), depth(depth),
                                                                 symbolsMap(symbolsMap), symbolsSet(symbolsSet) {}

VarInfo::VarInfo(const String &ident, const String &type, int depth) : ident(ident), type(type), depth(depth) {};
