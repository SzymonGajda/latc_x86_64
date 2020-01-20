//
// Created by szymon on 03.01.2020.
//

#ifndef LATC_X86_64_ALLOCATOR_H
#define LATC_X86_64_ALLOCATOR_H

#include <set>
#include <map>
#include "Absyn.H"
#include "BasicBlock.h"
#include "SymbolsTable.h"


class RegisterDescription {
public:
    int regNum;
    bool isFree;
    std::set<std::string> contents;
};

class Location {
public:
    Location();

    bool operator<(const Location &other) const;
    int reg;
};

class ValueDescription {
public:
    Ident ident;
    bool isValue;
    bool isSaved = true;
    int value;
    std::set<Location> locations;
};

class Allocator {
public:

    std::map<int, Ident> registersIdentMap;
    std::vector<String> code;
    int numOfRegisters = 14;
    std::map<int, RegisterDescription> registers;
    std::map<Ident, ValueDescription> values;
    std::map<Ident, LivenessInfo> livenessMap;
    SymbolsTable *symbolsTable;
    BasicBlock *actualBasicBlock;



    void initRegisters();

    void initRegistersIdentMap();

   // void writeLiveValues(std::map<Ident, LivenessInfo> liveVariables);

    void copy(Ident ident1, Ident ident2);

    void copy(Ident ident1, int val);

    void removeDeadValues(std::map<Ident, LivenessInfo> liveValues);

    void genAss1(Ident res, Ident arg, String op);

    void genAss1(Ident res, int val, String op);

    void genAss2(Ident res, int val1, int val2, String op);

    void genAss2(Ident res, Ident arg1, int val2, String op);

    void genAss2(Ident res, int val1, Ident arg2, String op);

    void genAss2(Ident res, Ident arg1, Ident arg2, String op);

    void genIf(Ident cond, Ident label1, Ident label2, std::map<Ident, LivenessInfo> liveVariables,
               std::map<Ident, int> actualBlockMemMap, std::map<Ident, int> nextBlockMemMapLabel1,
               std::map<Ident, int> nextBlockMemMapLabel2);

    void genRet(Ident retVal);

    void genRet(int retVal);

    void moveLocalVariables(std::map<Ident, LivenessInfo> liveVariables, std::map<Ident, int> actualBlockMemMap,
                            std::map<Ident, int> nextBlockMemMap);

    void initValues();

    int getFreeRegister(int exclude1, int exclude2);

    int getRegisterToSpill(int exclude1, int exclude2);

    void spillRegister(int reg);

  /*  int getFreeMem();

    void addLocationToValue(Ident ident, bool isInReg, int desc);

    void addContentToRegister(int reg, Ident ident);

    void addContentToMem(int reg, Ident ident);

    void removeLocationFromValue(Ident ident, bool isInReg, int desc);

    void removeContentFromRegister(int reg, Ident ident);

    void removeContentFromMem(int reg, Ident ident);*/
};


#endif //LATC_X86_64_ALLOCATOR_H
