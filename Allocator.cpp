//
// Created by szymon on 03.01.2020.
//

#include <iostream>
#include "Allocator.h"

void Allocator::copy(Ident ident1, Ident ident2) {
    ValueDescription valueDescription1 = values[ident1];
    ValueDescription valueDescription2 = values[ident2];

    for (Location location : valueDescription1.locations) {
        RegisterDescription registerDescription = registers.find(location.reg)->second;
        registerDescription.contents.erase(ident1);
        if (registerDescription.contents.empty()) {
            registerDescription.isFree = true;
        }
        registers.erase(location.reg);
        registers.emplace(location.reg, registerDescription);
    }
    valueDescription1.locations.clear();

    if (valueDescription2.isValue) {
        valueDescription1.isValue = true;
        valueDescription1.value = valueDescription2.value;
    } else {
        valueDescription1.isValue = false;
        valueDescription1.locations = valueDescription2.locations;
        for (Location location : valueDescription1.locations) {
            RegisterDescription registerDescription = registers.find(location.reg)->second;
            registerDescription.contents.insert(ident1);
            registers.erase(location.reg);
            registers.emplace(location.reg, registerDescription);
        }
    }
    values.erase(ident1);
    valueDescription1.isSaved = false;
    values.emplace(ident1, valueDescription1);
    if (!valueDescription1.isValue && valueDescription1.locations.empty()) {
        int reg = getFreeRegister(-1, -1);
        int varLocation = actualBasicBlock->memoryMap.find(ident2)->second;
        std::cout << "movq " << (varLocation + 1) * 8 << "(%ebp)" << ", %" << registersIdentMap.find(reg)->second
                  << "\n";
        registers.find(reg)->second.contents.insert(ident1);
        registers.find(reg)->second.contents.insert(ident2);
        registers.find(reg)->second.isFree = false;
        Location location;
        location.reg = reg;
        values.find(ident1)->second.locations.insert(location);
        values.find(ident2)->second.locations.insert(location);
        values.find(ident2)->second.isSaved = true;
        values.find(ident1)->second.isSaved = false;
    }
}

void Allocator::removeDeadValues(std::map<Ident, LivenessInfo> liveValues) {
    livenessMap = liveValues;
    for (auto value : values) {
        if (liveValues.find(value.first) == liveValues.end()) {
            ValueDescription valueDescription = value.second;
            for (Location location : valueDescription.locations) {
                RegisterDescription registerDescription = registers.find(location.reg)->second;
                registerDescription.contents.erase(value.first);
                if (registerDescription.contents.empty()) {
                    registerDescription.isFree = true;
                }
                registers.erase(location.reg);
                registers.emplace(location.reg, registerDescription);
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
    ValueDescription valueDescription1 = values[ident1];

    for (Location location : valueDescription1.locations) {
        RegisterDescription registerDescription = registers.find(location.reg)->second;
        registerDescription.contents.erase(ident1);
        if (registerDescription.contents.empty()) {
            registerDescription.isFree = true;
        }
        registers.erase(location.reg);
        registers.emplace(location.reg, registerDescription);
    }
    valueDescription1.locations.clear();
    valueDescription1.isValue = true;
    valueDescription1.value = val;
    valueDescription1.isSaved = false;
    values.erase(ident1);
    values.emplace(ident1, valueDescription1);
}

void Allocator::genAss1(Ident res, Ident arg, String op) {
    int resReg = -1;
    for (Location location : values.find(res)->second.locations) {
        registers.find(location.reg)->second.contents.erase(res);
        if (registers.find(location.reg)->second.contents.empty()) {
            registers.find(location.reg)->second.isFree = true;
        }
    }
    if (values.find(arg)->second.isValue) {
        resReg = getFreeRegister(-1, -1);
        std::cout << "movq $" << values.find(arg)->second.value << ", %" << registersIdentMap.find(resReg)->second
                  << "\n";
    } else {
        int argReg = -1;
        for (Location location : values.find(arg)->second.locations) {
            argReg = location.reg;
            if (registers.find(location.reg)->second.contents.size() == 1) {
                resReg = location.reg;
                break;
            }
        }
        if (resReg == -1) {
            resReg = getFreeRegister(argReg, -1);
            if (argReg != -1) {
                std::cout << "movq %" << registersIdentMap.find(argReg)->second << ", %"
                          << registersIdentMap.find(resReg)->second << "\n";
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg)->second;
                std::cout << "movq " << (varLocation + 1) * 8 << "(%ebp)" << ", %"
                          << registersIdentMap.find(resReg)->second << "\n";
            }
        }
    }
    std::cout << op << " %" << registersIdentMap.find(resReg)->second << "\n";
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
}

void Allocator::genAss1(Ident res, int val, String op) {
    int resReg;
    for (Location location : values.find(res)->second.locations) {
        registers.find(location.reg)->second.contents.erase(res);
        if (registers.find(location.reg)->second.contents.empty()) {
            registers.find(location.reg)->second.isFree = true;
        }
    }
    resReg = getFreeRegister(-1, -1);
    std::cout << "movq $" << val << ", %" << registersIdentMap.find(resReg)->second << "\n";
    std::cout << op << " %" << registersIdentMap.find(resReg)->second << "\n";
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
}

int Allocator::getFreeRegister(int exclude1, int exclude2) {
    for (auto reg : registers) {
        if ((reg.second.isFree) && (reg.second.regNum != exclude1) && (reg.second.regNum != exclude2)) {
            return reg.first;
        }
    }

    spillRegister(getRegisterToSpill(exclude1, exclude2));
    for (auto reg : registers) {
        if ((reg.second.isFree) && (reg.second.regNum != exclude1) && (reg.second.regNum != exclude2)) {
            return reg.first;
        }
    }
    return 0;
}

int Allocator::getRegisterToSpill(int exclude1, int exclude2) {
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
        if (localMin > globalMax && regToSpill != exclude1 && regToSpill != exclude2) {
            globalMax = localMin;
            regToSpill = reg.first;
        }
    }
    return regToSpill;
}

void Allocator::spillRegister(int reg) {
    for (auto val : registers.find(reg)->second.contents) {
        int varLocation = actualBasicBlock->memoryMap.find(val)->second;
        std::cout << "movq %" << registersIdentMap.find(reg)->second << ", " << (varLocation + 1) * 8 << "(%ebp)\n";
        for (auto it = values.find(val)->second.locations.begin();
             it != values.find(val)->second.locations.end(); it++) {
            if (it->reg == reg) {
                values.find(val)->second.locations.erase(it);
                break;
            }
        }
    }
    for (String val : registers.find(reg)->second.contents) {
        for (auto it = values.find(val)->second.locations.begin();
             it != values.find(val)->second.locations.end(); it++) {
            if (it->reg == reg) {
                values.find(val)->second.locations.erase(it);
                break;
            }
        }
    }
    registers.find(reg)->second.contents.clear();
    registers.find(reg)->second.isFree = true;
}


/*
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
*/
void Allocator::initRegistersIdentMap() {
    registersIdentMap.emplace(0, "rax");
    registersIdentMap.emplace(1, "rbx");
    registersIdentMap.emplace(2, "rcx");
    registersIdentMap.emplace(3, "rdx");
    registersIdentMap.emplace(4, "rsi");
    registersIdentMap.emplace(5, "rdi");
    registersIdentMap.emplace(6, "r8");
    registersIdentMap.emplace(7, "r9");
    registersIdentMap.emplace(8, "r10");
    registersIdentMap.emplace(9, "r11");
    registersIdentMap.emplace(10, "r12");
    registersIdentMap.emplace(11, "r13");
    registersIdentMap.emplace(12, "r14");
    registersIdentMap.emplace(13, "r15");
}

void Allocator::genAss2(Ident res, int val1, int val2, String op) {
    String relOp;
    if (op == "jl" || op == "jle" || op == "jg" || op == "jge" || op == "je" || op == "jne") {
        relOp = op;
        op = "cmp";
    }
    int resReg, argReg;
    for (Location location : values.find(res)->second.locations) {
        registers.find(location.reg)->second.contents.erase(res);
        if (registers.find(location.reg)->second.contents.empty()) {
            registers.find(location.reg)->second.isFree = true;
        }
    }
    if (op == "idiv" || op == "mod") {
        if (!registers.find(3)->second.isFree) {
            spillRegister(3);
        }
        if (!registers.find(0)->second.isFree) {
            spillRegister(0);
        }
        resReg = 0;
        argReg = getFreeRegister(0, 3);
    } else {
        resReg = getFreeRegister(-1, -1);
        argReg = getFreeRegister(resReg, -1);
    }
    std::cout << "movq $" << val1 << ", %" << registersIdentMap.find(resReg)->second << "\n";
    std::cout << "movq $" << val2 << ", %" << registersIdentMap.find(argReg)->second << "\n";
    if (op == "idiv" || op == "mod") {
        std::cout << "cdq\n";
        if (op == "idiv") {
            std::cout << op << " %" << registersIdentMap.find(argReg)->second
                      << "\n";
        } else {
            std::cout << "idiv" << " %" << registersIdentMap.find(argReg)->second
                      << "\n";
        }
    } else {
        std::cout << op << " %" << registersIdentMap.find(argReg)->second << ", %"
                  << registersIdentMap.find(resReg)->second
                  << "\n";
    }

    if (op == "mod") {
        resReg = 3;
    }
    if (op == "cmp") {
        Ident trueLabel = symbolsTable->getNewSymbol();
        Ident endLabel = symbolsTable->getNewSymbol();
        resReg = getFreeRegister(-1, -1);
        std::cout << relOp << " " << trueLabel << "\n";
        std::cout << "movq $0, %" << registersIdentMap.find(resReg)->second << "\n";
        std::cout << "jmp " << endLabel << "\n";
        std::cout << trueLabel << ":\n";
        std::cout << "movq $1, %" << registersIdentMap.find(resReg)->second << "\n";
        std::cout << endLabel << ":\n";

    }
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.isValue = false;
}

void Allocator::genAss2(Ident res, Ident arg1, int val2, String op) {
    String relOp;
    if (op == "jl" || op == "jle" || op == "jg" || op == "jge" || op == "je" || op == "jne") {
        relOp = op;
        op = "cmp";
    }
    if (values.find(res) == values.end()) {
        ValueDescription valueDescription;
        valueDescription.ident = res;
        valueDescription.isValue = false;
        values.emplace(res, valueDescription);
    }
    int resReg = -1;
    int argReg = -1;
    for (Location location : values.find(res)->second.locations) {
        registers.find(location.reg)->second.contents.erase(res);
        if (registers.find(location.reg)->second.contents.empty()) {
            registers.find(location.reg)->second.isFree = true;
        }
    }
    if (op == "idiv" || op == "mod") {
        if (!registers.find(3)->second.isFree) {
            spillRegister(3);
        }
        if (!registers.find(0)->second.isFree) {
            spillRegister(0);
        }
        resReg = 0;
        argReg = getFreeRegister(0, 3);
    }

    if (values.find(arg1)->second.isValue) {
        if (op == "idiv" || op == "mod") {
            std::cout << "movq $" << values.find(arg1)->second.value << ", %" << registersIdentMap.find(resReg)->second
                      << "\n";
            std::cout << "cdq\n";
        } else {
            resReg = getFreeRegister(-1, -1);
            std::cout << "movq $" << values.find(arg1)->second.value << ", %" << registersIdentMap.find(resReg)->second
                      << "\n";
        }
    } else {
        for (Location location : values.find(arg1)->second.locations) {
            argReg = location.reg;
            if (registers.find(location.reg)->second.contents.size() == 1 && resReg == -1) {
                resReg = location.reg;
                break;
            }
        }
        if (op == "idiv" || op == "mod") {
            if (argReg != -1) {
                std::cout << "movq %" << registersIdentMap.find(argReg)->second << ", %"
                          << registersIdentMap.find(resReg)->second << "\n";
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg1)->second;
                std::cout << "movq " << (varLocation + 1) * 8 << "(%ebp)" << ", %"
                          << registersIdentMap.find(resReg)->second << "\n";
            }
            std::cout << "cdq\n";
        }
        if (resReg == -1) {
            if (argReg != -1) {
                resReg = getFreeRegister(argReg, -1);
                std::cout << "movq %" << registersIdentMap.find(argReg)->second << ", %"
                          << registersIdentMap.find(resReg)->second << "\n";
            } else {
                resReg = getFreeRegister(-1, -1);
                int varLocation = actualBasicBlock->memoryMap.find(arg1)->second;
                std::cout << "movq " << (varLocation + 1) * 8 << "(%ebp)" << ", %"
                          << registersIdentMap.find(resReg)->second << "\n";
            }
        }
    }

    argReg = getFreeRegister(resReg, 3);
    std::cout << "movq $" << val2 << ", %" << registersIdentMap.find(argReg)->second
              << "\n";
    if (op == "idiv" || op == "mod") {
        if (op == "idiv") {
            std::cout << op << " %" << registersIdentMap.find(argReg)->second
                      << "\n";
        } else {
            std::cout << "idiv" << " %" << registersIdentMap.find(argReg)->second
                      << "\n";
        }
    } else {
        std::cout << op << " %" << registersIdentMap.find(argReg)->second << ", %"
                  << registersIdentMap.find(resReg)->second << "\n";
    }
    if (op == "mod") {
        resReg = 3;
    }
    if (op == "cmp") {
        Ident trueLabel = symbolsTable->getNewSymbol();
        Ident endLabel = symbolsTable->getNewSymbol();
        resReg = getFreeRegister(-1, -1);
        std::cout << relOp << " " << trueLabel << "\n";
        std::cout << "movq $0, %" << registersIdentMap.find(resReg)->second << "\n";
        std::cout << "jmp " << endLabel << "\n";
        std::cout << trueLabel << ":\n";
        std::cout << "movq $1, %" << registersIdentMap.find(resReg)->second << "\n";
        std::cout << endLabel << ":\n";

    }
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.isValue = false;
}

void Allocator::genAss2(Ident res, int val1, Ident arg2, String op) {
    String relOp;
    if (op == "jl" || op == "jle" || op == "jg" || op == "jge" || op == "je" || op == "jne") {
        relOp = op;
        op = "cmp";
    }

    int resReg = -1;
    int argReg = -1;
    for (Location location : values.find(res)->second.locations) {
        registers.find(location.reg)->second.contents.erase(res);
        if (registers.find(location.reg)->second.contents.empty()) {
            registers.find(location.reg)->second.isFree = true;
        }
    }

    if (op == "idiv" || op == "mod") {
        if (!registers.find(3)->second.isFree) {
            spillRegister(3);
        }
        if (!registers.find(0)->second.isFree) {
            spillRegister(0);
        }
        resReg = 0;
        std::cout << "movq $" << val1 << ", %" << registersIdentMap.find(resReg)->second
                  << "\n";
        std::cout << "cdq\n";
    } else {
        resReg = getFreeRegister(-1, -1);
        std::cout << "movq $" << val1 << ", %" << registersIdentMap.find(resReg)->second
                  << "\n";
    }
    if (values.find(arg2)->second.isValue) {
        if (op == "idiv" || op == "mod") {
            argReg = getFreeRegister(0, 3);
            std::cout << "movq $" << values.find(arg2)->second.value << ", %" << registersIdentMap.find(argReg)->second
                      << "\n";
            std::cout << "idiv" << " %" << registersIdentMap.find(argReg)->second << "\n";
        } else {
            argReg = getFreeRegister(resReg, -1);
            std::cout << "movq $" << values.find(arg2)->second.value << ", %" << registersIdentMap.find(argReg)->second
                      << "\n";
            std::cout << op << " %" << registersIdentMap.find(argReg)->second << ", %"
                      << registersIdentMap.find(resReg)->second << "\n";
        }
    } else {
        argReg = -1;
        for (Location location : values.find(arg2)->second.locations) {
            argReg = location.reg;
        }
        if (argReg != -1) {
            if (op == "idiv" || op == "mod") {
                std::cout << "idiv" << " %" << registersIdentMap.find(argReg)->second << "\n";
            } else {
                std::cout << op << " %" << registersIdentMap.find(argReg)->second << ", %"
                          << registersIdentMap.find(resReg)->second << "\n";
            }
        } else {
            if (op == "idiv" || op == "mod") {
                std::cout << "idiv" << " %" << registersIdentMap.find(argReg)->second << "\n";
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg2)->second;
                std::cout << op << " " << (varLocation + 1) * 8 << "(%ebp)" << ", %"
                          << registersIdentMap.find(resReg)->second << "\n";
            }
        }
    }
    if (op == "mod") {
        resReg = 3;
    }
    if (op == "cmp") {
        Ident trueLabel = symbolsTable->getNewSymbol();
        Ident endLabel = symbolsTable->getNewSymbol();
        resReg = getFreeRegister(-1, -1);
        std::cout << relOp << " " << trueLabel << "\n";
        std::cout << "movq $0, %" << registersIdentMap.find(resReg)->second << "\n";
        std::cout << "jmp " << endLabel << "\n";
        std::cout << trueLabel << ":\n";
        std::cout << "movq $1, %" << registersIdentMap.find(resReg)->second << "\n";
        std::cout << endLabel << ":\n";

    }
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.isValue = false;
}

void Allocator::genAss2(Ident res, Ident arg1, Ident arg2, String op) {
    String relOp;
    if (op == "jl" || op == "jle" || op == "jg" || op == "jge" || op == "je" || op == "jne") {
        relOp = op;
        op = "cmp";
    }
    int resReg = -1;
    int argReg = -1;
    for (Location location : values.find(res)->second.locations) {
        registers.find(location.reg)->second.contents.erase(res);
        if (registers.find(location.reg)->second.contents.empty()) {
            registers.find(location.reg)->second.isFree = true;
        }
    }

    if (values.find(arg1)->second.isValue) {
        if (op == "idiv" || op == "mod") {
            if (!registers.find(3)->second.isFree) {
                spillRegister(3);
            }
            if (!registers.find(0)->second.isFree) {
                spillRegister(0);
            }
            resReg = 0;
            std::cout << "movq $" << values.find(arg1)->second.value << ", %" << registersIdentMap.find(resReg)->second
                      << "\n";
            std::cout << "cdq\n";
        } else {
            resReg = getFreeRegister(-1, -1);
            std::cout << "movq $" << values.find(arg1)->second.value << ", %" << registersIdentMap.find(resReg)->second
                      << "\n";
        }
    } else {
        for (Location location : values.find(arg1)->second.locations) {
            argReg = location.reg;
            if (registers.find(location.reg)->second.contents.size() == 1) {
                resReg = location.reg;
                break;
            }
        }
        if (resReg == -1) {
            if (op == "idiv" || op == "mod") {
                if (!registers.find(3)->second.isFree) {
                    spillRegister(3);
                }
                if (!registers.find(0)->second.isFree) {
                    spillRegister(0);
                }
                resReg = 0;
                if (argReg != -1) {
                    std::cout << "movq %" << registersIdentMap.find(argReg)->second << ", %"
                              << registersIdentMap.find(resReg)->second << "\n";
                } else {
                    int varLocation = actualBasicBlock->memoryMap.find(arg1)->second;
                    std::cout << "movq " << (varLocation + 1) * 8 << "(%ebp)" << ", %"
                              << registersIdentMap.find(resReg)->second << "\n";
                }
                std::cout << "cdq\n";
            } else {
                resReg = getFreeRegister(-1, -1);
                if (argReg != -1) {
                    std::cout << "movq %" << registersIdentMap.find(argReg)->second << ", %"
                              << registersIdentMap.find(resReg)->second << "\n";
                } else {
                    int varLocation = actualBasicBlock->memoryMap.find(arg1)->second;
                    std::cout << "movq " << (varLocation + 1) * 8 << "(%ebp)" << ", %"
                              << registersIdentMap.find(resReg)->second << "\n";
                }
            }
        }
    }

    if (values.find(arg2)->second.isValue) {
        if (op == "idiv" || op == "mod") {
            argReg = getFreeRegister(resReg, 3);
            std::cout << "movq $" << values.find(arg2)->second.value << ", %" << registersIdentMap.find(argReg)->second
                      << "\n";
            std::cout << "idiv" << " %" << registersIdentMap.find(argReg)->second << "\n";
        } else {
            argReg = getFreeRegister(resReg, -1);
            std::cout << "movq $" << values.find(arg2)->second.value << ", %" << registersIdentMap.find(argReg)->second
                      << "\n";
            std::cout << op << " %" << registersIdentMap.find(argReg)->second << ", %"
                      << registersIdentMap.find(resReg)->second << "\n";
        }
    } else {
        argReg = -1;
        for (Location location : values.find(arg2)->second.locations) {
            argReg = location.reg;
        }
        if (op == "idiv" || op == "mod") {
            if (argReg != -1) {
                std::cout << "idiv" << " %" << registersIdentMap.find(argReg)->second << "\n";
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg2)->second;
                std::cout << "idivq " << (varLocation + 1) * 8 << "(%ebp)" << "\n";
            }
        } else {
            if (argReg != -1) {
                std::cout << op << " %" << registersIdentMap.find(argReg)->second << ", %"
                          << registersIdentMap.find(resReg)->second << "\n";
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg2)->second;
                std::cout << op << " " << (varLocation + 1) * 8 << "(%ebp), %" << registersIdentMap.find(resReg)->second
                          << "\n";
            }
        }
    }
    if (op == "mod") {
        resReg = 3;
    }
    if (op == "cmp") {
        Ident trueLabel = symbolsTable->getNewSymbol();
        Ident endLabel = symbolsTable->getNewSymbol();
        resReg = getFreeRegister(-1, -1);
        std::cout << relOp << " " << trueLabel << "\n";
        std::cout << "movq $0, %" << registersIdentMap.find(resReg)->second << "\n";
        std::cout << "jmp " << endLabel << "\n";
        std::cout << trueLabel << ":\n";
        std::cout << "movq $1, %" << registersIdentMap.find(resReg)->second << "\n";
        std::cout << endLabel << ":\n";

    }
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.isValue = false;
}

void Allocator::genRet(Ident retVal) {
    if (values.find(retVal)->second.isValue) {
        std::cout << "movl $" << values.find(retVal)->second.value << ", %eax\n";
    } else {
        int retReg = -1;
        for (Location location : values.find(retVal)->second.locations) {
            retReg = location.reg;
        }
        if (retReg == -1) {
            int varLocation = actualBasicBlock->memoryMap.find(retVal)->second;
            std::cout << "movq " << (varLocation + 1) * 8 << "(%ebp), %rax\n";
        } else {
            std::cout << "movq %" << registersIdentMap.find(retReg)->second << ", %rax\n";
        }
    }
    std::cout << "ret\n";
}

void Allocator::genRet(int retVal) {
    std::cout << "movl $" << retVal << ", %eax\nret\n";
}

void Allocator::genIf(Ident cond, Ident label1, Ident label2, std::map<Ident, LivenessInfo> liveVariables,
                      std::map<Ident, int> actualBlockMemMap, std::map<Ident, int> nextBlockMemMapLabel1,
                      std::map<Ident, int> nextBlockMemMapLabel2) {
    if (values.find(cond)->second.isValue) {
        if (values.find(cond)->second.value == 0) {
            // writeLiveValues();
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel2);
            std::cout << "jmp " << label2 << "\n";
        } else {
            // writeLiveValues();
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel1);
            std::cout << "jmp " << label1 << "\n";
        }
    } else {
        int retReg = -1;
        for (Location location : values.find(cond)->second.locations) {
            retReg = location.reg;
            if (registers.find(location.reg)->second.contents.size() == 1) {
                retReg = location.reg;
                break;
            }
        }
        Ident l1, l2;
        l1 = symbolsTable->getNewSymbol();
        l2 = symbolsTable->getNewSymbol();
        if (retReg == -1) {
            int varLocation = actualBasicBlock->memoryMap.find(cond)->second;
            std::cout << "cmp $0, " << (varLocation + 1) * 8 << "(%ebp)\n";
            std::cout << "je " << l2 << "\n";
            std::cout << "jne" << l1 << "\n";
            std::cout << l2 << ": \n";
            // writeLiveValues();
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel2);
            std::cout << "jmp " << label2 << "\n";
            std::cout << l1 << ":\n";
            // writeLiveValues();
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel1);
            std::cout << "jmp " << label1 << "\n";
        } else {
            std::cout << "cmp $0, %" << registersIdentMap.find(retReg)->second << "\n";
            std::cout << "je " << l2 << "\n";
            std::cout << "jne " << l1 << "\n";
            std::cout << l2 << ": \n";
            //writeLiveValues();
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel2);
            std::cout << "jmp " << label2 << "\n";
            std::cout << l1 << ":\n";
            // writeLiveValues();
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel1);
            std::cout << "jmp " << label1 << "\n";
        }
    }
}

/*void Allocator::writeLiveValues(std::map<Ident, LivenessInfo> liveVariables) {
    for (auto it = values.begin(); it != values.end(); it++) {
        if (it->second.isValue) {*/
/* int reg = getFreeRegister(-1, -1);
 std::cout << "movq $" << it->second.value << ", %" << registersIdentMap.find(reg)->second << "\n";
 Location location;
 location.reg = reg;
 it->second.isValue = false;
 it->second.isSaved = true;
 it->second.locations.insert(location);
 registers.find(reg)->second.isFree = false;
 registers.find(reg)->second.contents.insert(it->second.ident);*/
/*   int varLocation = actualBasicBlock->memoryMap.find(it->second.ident)->second;
   std::cout << "movq  $"<<it->second.value<<", "<<(varLocation + 1) * 8 << "(%ebp)\n";
   it->second.isValue = false;
   it->second.isSaved = true;
}
if (!it->second.isValue && !it->second.isSaved) {
   int oldReg = -1;
   for (auto loc : it->second.locations) {
       oldReg = loc.reg;
       break;
   }
   int varLocation = actualBasicBlock->memoryMap.find(it->second.ident)->second;
   std::cout << "movq  %"<<registersIdentMap.find(oldReg)->second<<", "<<(varLocation + 1) * 8 << "(%ebp)\n";*/
/* if (oldReg != -1) {
     registers.find(oldReg)->second.contents.erase(it->first);
     if (registers.find(oldReg)->second.contents.empty()) {
         registers.find(oldReg)->second.isFree = true;
     }
     std::cout << "movq %" << registersIdentMap.find(oldReg)->second << ", %"
               << registersIdentMap.find(reg)->second << "\n";
 } else {
     memory.find(oldMem)->second.contents.erase(it->first);
     if (memory.find(oldMem)->second.contents.empty()) {
         memory.find(oldMem)->second.isFree = true;
     }
     std::cout << "movq [" << oldMem << "], %" << registersIdentMap.find(reg)->second << "\n";
 }*/

/* it->second.isValue = false;
 it->second.isSaved = true;
}
}
}*/

void Allocator::moveLocalVariables(std::map<Ident, LivenessInfo> liveVariables, std::map<Ident, int> actualBlockMemMap,
                                   std::map<Ident, int> nextBlockMemMap) {
    int movRegister = -1;
    int actualVarLocation, nextVarLocation;
    for (auto live : liveVariables) {
        if (nextBlockMemMap.find(live.second.ident) != nextBlockMemMap.end()) {
            if (values.find(live.second.ident)->second.isValue) {
                nextVarLocation = nextBlockMemMap.find(live.first)->second;
                std::cout << "movq $" << values.find(live.second.ident)->second.value << ", "
                          << (nextVarLocation + 1) * 8 << "(%ebp)\n";
            } else if (!values.find(live.second.ident)->second.isValue &&
                       !values.find(live.second.ident)->second.locations.empty()) {
                nextVarLocation = nextBlockMemMap.find(live.first)->second;
                std::cout << "movq %" << registersIdentMap.find(
                        values.find(live.second.ident)->second.locations.begin()->reg)->second << ", "
                          << (nextVarLocation + 1) * 8 << "(%ebp)\n";
            } else {
                actualVarLocation = actualBlockMemMap.find(live.first)->second;
                nextVarLocation = nextBlockMemMap.find(live.first)->second;
                if (actualVarLocation != nextVarLocation) {
                    if (movRegister == -1) {
                        movRegister = getFreeRegister(-1, -1);
                    }
                    std::cout << "movq " << (actualVarLocation + 1) * 8 << "(%ebp), %"
                              << registersIdentMap.find(movRegister)->second << "\n";
                    std::cout << "movq %" << registersIdentMap.find(movRegister)->second << ", "
                              << (nextVarLocation + 1) * 8 << "(%ebp)\n";
                }
            }
        }
    }

}

void Allocator::initValues() {
    values.clear();
    for (auto var : actualBasicBlock->memoryMap) {
        ValueDescription valueDescription;
        valueDescription.ident = var.first;
        valueDescription.isValue = false;
        valueDescription.isSaved = true;
        valueDescription.locations.clear();
        values.emplace(var.first, valueDescription);
    }
}


Location::Location() {}

bool Location::operator<(const Location &other) const {
    return other.reg > this->reg;
}
