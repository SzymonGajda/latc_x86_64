//
// Created by szymon on 03.01.2020.
//

#ifndef LATC_X86_64_ALLOCATOR_H
#define LATC_X86_64_ALLOCATOR_H

#include <set>
#include <map>
#include "Absyn.H"
#include "BasicBlock.h"


class RegisterDescription{
public:
    int regNum;
    bool isFree;
    std::set<std::string> contents;
};

class MemDecription{
public:
    int memNum;
    bool isFree;
    std::set<std::string> contents;
};

class Location{
public:
    bool isInRegister;
    bool isInMemory;
    int reg;
    int mem;
};

class ValueDescription{
public:
    Ident ident;
    bool isValue;
    int value;
    std::set<Location> locations;
};

class Allocator {
public:

    std::vector<String> code;
    int numOfRegisters = 5;
    std::map<int, MemDecription> memory;
    std::map<int, RegisterDescription> registers;
    std::map<Ident, ValueDescription> values;
    std::map<Ident, LivenessInfo> livenessMap;

    void initRegisters();
    void copy(Ident ident1, Ident ident2);
    void copy(Ident ident1, int val);
    void removeDeadValues(std::map<Ident, LivenessInfo> liveValues);
    void genAss1(Ident res, Ident arg, String op);
    int getFreeRegister();
    int getRegisterToSpill();
    void spillRegister(int reg);
    int getFreeMem();
    void addLocationToValue(Ident ident, bool isInReg, int desc);
    void addContentToRegister(int reg, Ident ident);
    void addContentToMem(int reg, Ident ident);
    void removeLocationFromValue(Ident ident, bool isInReg, int desc);
    void removeContentFromRegister(int reg, Ident ident);
    void removeContentFromMem(int reg, Ident ident);
};


#endif //LATC_X86_64_ALLOCATOR_H
