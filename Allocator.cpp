//
// Created by szymon on 03.01.2020.
//

#include <iostream>
#include "Allocator.h"

void Allocator::copy(Ident ident1, Ident ident2) {
    if(values.find(ident1) == values.end()){
        ValueDescription valueDescription;
        valueDescription.ident = ident1;
        valueDescription.isValue = false;
        values.emplace(ident1, valueDescription);
    }
    ValueDescription valueDescription1 = values[ident1];
    ValueDescription valueDescription2 = values[ident2];

    for (Location location : valueDescription1.locations) {
        if (location.isInMemory) {
            MemDecription memDecription = memory.find(location.mem)->second;
            memDecription.contents.erase(ident1);
            if (memDecription.contents.empty()) {
                memDecription.isFree = true;
            }
            memory.erase(location.mem);
            memory.emplace(location.mem, memDecription);
        }
        if (location.isInRegister) {
            RegisterDescription registerDescription = registers.find(location.reg)->second;
            registerDescription.contents.erase(ident1);
            if (registerDescription.contents.empty()) {
                registerDescription.isFree = true;
            }
            registers.erase(location.reg);
            registers.emplace(location.reg, registerDescription);
        }
    }
    valueDescription1.locations.clear();

    if (valueDescription2.isValue) {
        valueDescription1.isValue = true;
        valueDescription1.value = valueDescription2.value;
    } else {
        valueDescription1.locations = valueDescription2.locations;
        for (Location location : valueDescription1.locations) {
            if (location.isInMemory) {
                MemDecription memDecription = memory.find(location.mem)->second;
                memDecription.contents.insert(ident1);
                memory.erase(location.mem);
                memory.emplace(location.mem, memDecription);
            }
            if (location.isInRegister) {
                RegisterDescription registerDescription = registers.find(location.reg)->second;
                registerDescription.contents.insert(ident1);
                registers.erase(location.reg);
                registers.emplace(location.reg, registerDescription);
            }
        }
    }
    values.erase(ident1);
    values.emplace(ident1, valueDescription1);
}

void Allocator::removeDeadValues(std::map<Ident, LivenessInfo> liveValues) {
    livenessMap = liveValues;
    for (auto value : values) {
        if (liveValues.find(value.first) == liveValues.end()) {
            ValueDescription valueDescription = value.second;
            for (Location location : valueDescription.locations) {
                if (location.isInRegister) {
                    RegisterDescription registerDescription = registers.find(location.reg)->second;
                    registerDescription.contents.erase(value.first);
                    if (registerDescription.contents.empty()) {
                        registerDescription.isFree = true;
                    }
                    registers.erase(location.reg);
                    registers.emplace(location.reg, registerDescription);
                }
                if (location.isInMemory) {
                    MemDecription memDecription = memory.find(location.mem)->second;
                    memDecription.contents.erase(value.first);
                    if (memDecription.contents.empty()) {
                        memDecription.isFree = true;
                    }
                    memory.erase(location.mem);
                    memory.emplace(location.mem, memDecription);
                }
            }
            value.second.locations.clear();
            value.second.isValue = false;
        }
    }
}

void Allocator::initRegisters() {
    for (int i = 0; i < numOfRegisters; i++) {
        RegisterDescription registerDescription;
        registerDescription.isFree = true;
        registerDescription.regNum = i;
        registers.emplace(i, registerDescription);
    }
}

void Allocator::copy(Ident ident1, int val) {
    if(values.find(ident1) == values.end()){
        ValueDescription valueDescription;
        valueDescription.ident = ident1;
        valueDescription.isValue = false;
        values.emplace(ident1, valueDescription);
    }
    ValueDescription valueDescription1 = values[ident1];

    for (Location location : valueDescription1.locations) {
        if (location.isInMemory) {
            MemDecription memDecription = memory.find(location.mem)->second;
            memDecription.contents.erase(ident1);
            if (memDecription.contents.empty()) {
                memDecription.isFree = true;
            }
            memory.erase(location.mem);
            memory.emplace(location.mem, memDecription);
        }
        if (location.isInRegister) {
            RegisterDescription registerDescription = registers.find(location.reg)->second;
            registerDescription.contents.erase(ident1);
            if (registerDescription.contents.empty()) {
                registerDescription.isFree = true;
            }
            registers.erase(location.reg);
            registers.emplace(location.reg, registerDescription);
        }
    }
    valueDescription1.locations.clear();
    valueDescription1.isValue = true;
    valueDescription1.value = val;
    values.erase(ident1);
    values.emplace(ident1, valueDescription1);
}

void Allocator::genAss1(Ident res, Ident arg, String op) {
    if(values.find(res) == values.end()){
        ValueDescription valueDescription;
        valueDescription.ident = res;
        valueDescription.isValue = false;
        values.emplace(res, valueDescription);
    }
    int resReg = -1;
    for(Location location : values.find(res)->second.locations){
        if(location.isInMemory){
            memory.find(location.mem)->second.contents.erase(res);
            if(memory.find(location.mem)->second.contents.empty()){
                memory.find(location.mem)->second.isFree = true;
            }
        }
        if(location.isInRegister){
            registers.find(location.reg)->second.contents.erase(res);
            if(registers.find(location.reg)->second.contents.empty()){
                registers.find(location.reg)->second.isFree = true;
            }
        }
    }
    if (values.find(arg)->second.isValue) {
        resReg = getFreeRegister();
        std::cout<<"MOV "<<values.find(arg)->second.value<<" "<<resReg<<"\n";
    } else{
        int argReg = -1;
        int argMem = -1;
        for(Location location : values.find(arg)->second.locations){
            if(location.isInRegister){
                argReg = location.reg;
            }
            else{
                argMem = location.mem;
            }
            if(location.isInRegister && registers.find(location.reg)->second.contents.size() == 1){
                resReg = location.reg;
                break;
            }
        }
        if(resReg == -1){
            resReg = getFreeRegister();
            if(argReg != -1) {
                std::cout << "MOV " <<argReg<<" "<<resReg<<"\n";
            }
            else{
                std::cout << "MOV [" <<argMem<<"] "<<resReg<<"\n";
            }
        }
    }
    std::cout<<"op"<<" "<<resReg<<"\n";
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    Location location1;
    location1.isInRegister = true;
    location1.isInMemory = false;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
}

int Allocator::getFreeRegister() {
    for (auto reg : registers) {
        if (reg.second.isFree) {
            return reg.first;
        }
    }

    spillRegister(getRegisterToSpill());
    for (auto reg : registers) {
        if (reg.second.isFree) {
            return reg.first;
        }
    }
    return 0;
}

int Allocator::getRegisterToSpill() {
    int regToSpill = 0;
    int globalMax = 0;
    int localMin;
    for (auto reg : registers) {
        localMin = 1000000000;
        for (Ident ident : reg.second.contents) {
            if (livenessMap.find(ident)->second.nextUse < localMin) {
                localMin = livenessMap.find(ident)->second.nextUse;
            }
        }
        if (localMin > globalMax) {
            globalMax = localMin;
            regToSpill = reg.first;
        }
    }
    return regToSpill;
}

void Allocator::spillRegister(int reg) {
    int mem = getFreeMem();
    std::cout << "MOV " << reg << " " << mem << "\n";
    for (String val : registers.find(reg)->second.contents) {
        for (auto it = values.find(val)->second.locations.begin();
             it != values.find(val)->second.locations.end(); it++) {
            if (it->isInRegister && it->reg == reg) {
                values.find(val)->second.locations.erase(it);
                break;
            }
        }
    }
    MemDecription memDecription;
    memDecription.memNum = mem;
    memDecription.isFree = false;
    memDecription.contents = registers.find(reg)->second.contents;
    memory.emplace(mem, memDecription);
    registers.find(reg)->second.contents.clear();
    registers.find(reg)->second.isFree = true;
    ValueDescription valueDescription;
    Location location;
    location.isInRegister = false;
    location.isInMemory = true;
    location.mem = mem;
    for (String val : memDecription.contents) {
        values.find(val)->second.locations.insert(location);
    }
}

int Allocator::getFreeMem() {
    for (auto mem : memory) {
        if (mem.second.isFree) {
            return mem.second.memNum;
        }
    }
    MemDecription memDecription;
    memDecription.isFree = true;
    memDecription.memNum = memory.size();
    memory.emplace(memDecription.memNum, memDecription);
    return memDecription.memNum;
}

void Allocator::addLocationToValue(Ident ident, bool isInReg, int desc) {
    Location location;
    location.isInMemory = !isInReg;
    location.isInRegister = isInReg;
    if (isInReg) {
        location.reg = desc;
    } else {
        location.mem = desc;
    }
    ValueDescription valueDescription = values[ident];
    valueDescription.locations.insert(location);
    values.erase(ident);
    values.emplace(ident, valueDescription);
}

void Allocator::addContentToRegister(int reg, Ident ident) {
    registers.find(reg)->second.contents.insert(ident);
    registers.find(reg)->second.isFree = false;
}

void Allocator::addContentToMem(int reg, Ident ident) {
    memory.find(reg)->second.contents.insert(ident);
    memory.find(reg)->second.isFree = false;
}

void Allocator::removeLocationFromValue(Ident ident, bool isInReg, int desc) {
    ValueDescription valueDescription = values.find(ident)->second;
    values.erase(ident);
    for (auto it = valueDescription.locations.begin(); it != valueDescription.locations.end(); it++) {
        if ((it->isInRegister && isInReg) || (!isInReg && it->isInMemory)) {
            valueDescription.locations.erase(it);
            break;
        }
    }
    values.emplace(ident, valueDescription);
}

void Allocator::removeContentFromRegister(int reg, Ident ident) {
    registers.find(reg)->second.contents.erase(ident);
    if (registers.find(reg)->second.contents.empty()) {
        registers.find(reg)->second.isFree = true;
    }
}

void Allocator::removeContentFromMem(int reg, Ident ident) {
    memory.find(reg)->second.contents.erase(ident);
    if (memory.find(reg)->second.contents.empty()) {
        memory.find(reg)->second.isFree = true;
    }
}









