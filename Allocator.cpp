//
// Created by szymon on 03.01.2020.
//

#include <iostream>
#include "Allocator.h"

void Allocator::copy(Ident ident1, Ident ident2) {
    ValueDescription valueDescription1 = values[ident1];
    ValueDescription valueDescription2 = values[ident2];

    if (valueDescription1.locations.empty() && !valueDescription1.isValue &&
        (registerAllocationMap.find(ident1) == registerAllocationMap.end() ||
         registerAllocationMap.find(ident1)->second == -1)) {
        return;
    }

    if (valueDescription2.locations.empty() && !valueDescription2.isValue &&
        (registerAllocationMap.find(ident2) == registerAllocationMap.end() ||
         registerAllocationMap.find(ident2)->second == -1)) {
        return;
    }

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
        int reg;
        if (registerAllocationMap.find(ident1)->second == -1) {
            reg = getFreeRegister(-1, -1);
        } else {
            reg = registerAllocationMap.find(ident1)->second;
        }
        if (registerAllocationMap.find(ident2)->second != -1) {
            resultCode += "movq %" + registersIdentMap.find(registerAllocationMap.find(ident2)->second)->second + ", %"
                          + registersIdentMap.find(reg)->second
                          + "\n";
        } else {
            int varLocation = actualBasicBlock->memoryMap.find(ident2)->second;
            resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + ", %" +
                          registersIdentMap.find(reg)->second
                          + "\n";
        }
        registers.find(reg)->second.contents.insert(ident1);
        registers.find(reg)->second.contents.insert(ident2);
        registers.find(reg)->second.isFree = false;
        Location location;
        location.reg = reg;
        values.find(ident1)->second.locations.insert(location);
        values.find(ident2)->second.locations.insert(location);
        values.find(ident2)->second.isSaved = true;
        values.find(ident1)->second.isSaved = false;
        values.find(ident1)->second.type = values.find(ident2)->second.type;
    }
    saveValue(ident1);
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
    for (int i = 0; i < 14; i++) {
        RegisterDescription registerDescription;
        registerDescription.isFree = true;
        registerDescription.regNum = i;
        registers.emplace(i, registerDescription);
    }
}

void Allocator::copy(Ident ident1, int val) {
    ValueDescription valueDescription1 = values[ident1];

    if (valueDescription1.locations.empty() && !valueDescription1.isValue &&
        (registerAllocationMap.find(ident1) == registerAllocationMap.end() ||
         registerAllocationMap.find(ident1)->second == -1)) {
        return;
    }

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
    valueDescription1.type = "int";
    values.erase(ident1);
    values.emplace(ident1, valueDescription1);
    saveValue(ident1);
}

void Allocator::copyString(Ident ident1, String val) {
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
    int reg;
    if (registerAllocationMap.find(ident1)->second == -1) {
        reg = getFreeRegister(-1, -1);
    } else {
        reg = registerAllocationMap.find(ident1)->second;
    }
    resultCode += "lea " + val + "(%rip), %" + registersIdentMap.find(reg)->second + "\n";
    valueDescription1.locations.clear();
    Location location1;
    location1.reg = reg;
    registers.find(reg)->second.isFree = false;
    registers.find(reg)->second.contents.insert(ident1);
    valueDescription1.locations.insert(location1);
    valueDescription1.isValue = false;
    valueDescription1.isSaved = false;
    valueDescription1.type = "string";
    values.erase(ident1);
    values.emplace(ident1, valueDescription1);
    saveValue(ident1);
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
        if (registerAllocationMap.find(res)->second == -1) {
            resReg = getFreeRegister(-1, -1);
        } else {
            resReg = registerAllocationMap.find(res)->second;
        }
        resultCode += "movq $" + std::to_string(values.find(arg)->second.value) + ", %" +
                      registersIdentMap.find(resReg)->second
                      + "\n";
    } else {
        int argReg = -1;
        for (Location location : values.find(arg)->second.locations) {
            argReg = location.reg;
            if (registers.find(location.reg)->second.contents.size() == 1 && values.find(arg)->second.isSaved) {
                resReg = location.reg;
                break;
            }
        }
        if (registerAllocationMap.find(arg)->second != -1) {
            argReg = registerAllocationMap.find(arg)->second;
        }
        if (registerAllocationMap.find(res)->second != -1) {
            resReg = registerAllocationMap.find(res)->second;
            if (argReg != -1) {
                if (argReg != resReg) {
                    resultCode += "movq %" + registersIdentMap.find(argReg)->second + ", %"
                                  + registersIdentMap.find(resReg)->second + "\n";
                }
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg)->second;
                resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + ", %"
                              + registersIdentMap.find(resReg)->second + "\n";
            }
        } else {
            if (resReg == -1) {
                resReg = getFreeRegister(argReg, -1);
                if (argReg != -1) {
                    resultCode += "movq %" + registersIdentMap.find(argReg)->second + ", %"
                                  + registersIdentMap.find(resReg)->second + "\n";
                } else {
                    int varLocation = actualBasicBlock->memoryMap.find(arg)->second;
                    resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + ", %"
                                  + registersIdentMap.find(resReg)->second + "\n";
                }
            }
        }
    }
    resultCode += op + " %" + registersIdentMap.find(resReg)->second + "\n";
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    values.find(res)->second.isSaved = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.type = "int";
    saveValue(res);
}

void Allocator::genAss1(Ident res, int val, String op) {
    int resReg;
    for (Location location : values.find(res)->second.locations) {
        registers.find(location.reg)->second.contents.erase(res);
        if (registers.find(location.reg)->second.contents.empty()) {
            registers.find(location.reg)->second.isFree = true;
        }
    }
    if (registerAllocationMap.find(res)->second == -1) {
        resReg = getFreeRegister(-1, -1);
    } else {
        resReg = registerAllocationMap.find(res)->second;
    }
    resultCode += "movq $" + std::to_string(val) + ", %" + registersIdentMap.find(resReg)->second + "\n";
    resultCode += op + " %" + registersIdentMap.find(resReg)->second + "\n";
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    values.find(res)->second.isSaved = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.type = "int";
    saveValue(res);
}

int Allocator::getFreeRegister(int exclude1, int exclude2) {
    for (auto reg : registers) {
        if ((reg.second.isFree) && (reg.second.regNum != exclude1) && (reg.second.regNum != exclude2) &&
            reg.first < numOfRegisters) {
            return reg.first;
        }
    }

    spillRegister(getRegisterToSpill(exclude1, exclude2));
    for (auto reg : registers) {
        if ((reg.second.isFree) && (reg.second.regNum != exclude1) && (reg.second.regNum != exclude2) &&
            reg.first < numOfRegisters) {
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
        if (reg.first < numOfRegisters) {
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
    }
    return regToSpill;
}

void Allocator::spillRegister(int reg) {
    if (reg < numOfRegisters) {
        for (auto val : registers.find(reg)->second.contents) {
            if (registerAllocationMap.find(val)->second != -1 && !values.find(val)->second.isSaved) {
                resultCode += "movq %" + registersIdentMap.find(reg)->second + ", %"
                              + registersIdentMap.find(registerAllocationMap.find(val)->second)->second + "\n";
            } else if (actualBasicBlock->memoryMap.find(val) != actualBasicBlock->memoryMap.end() &&
                       !values.find(val)->second.isSaved) {
                int varLocation = actualBasicBlock->memoryMap.find(val)->second;
                resultCode +=
                        "movq %" + registersIdentMap.find(reg)->second + ", " + std::to_string((varLocation + 1) * -8)
                        + "(%rbp)\n";
                for (auto it = values.find(val)->second.locations.begin();
                     it != values.find(val)->second.locations.end(); it++) {
                    if (it->reg == reg) {
                        values.find(val)->second.locations.erase(it);
                        break;
                    }
                }
                values.find(val)->second.isSaved = true;
            }
        }

        registers.find(reg)->second.contents.clear();
        registers.find(reg)->second.isFree = true;
    }
}

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
        if (registerAllocationMap.find(res)->second == -1) {
            resReg = getFreeRegister(-1, -1);
        } else {
            resReg = registerAllocationMap.find(res)->second;
        }
        argReg = getFreeRegister(resReg, -1);
    }
    resultCode += "movq $" + std::to_string(val1) + ", %" + registersIdentMap.find(resReg)->second + "\n";
    resultCode += "movq $" + std::to_string(val2) + ", %" + registersIdentMap.find(argReg)->second + "\n";
    if (op == "idiv" || op == "mod") {
        resultCode += "cqo\n";
        if (op == "idiv") {
            resultCode += op + " %" + registersIdentMap.find(argReg)->second
                          + "\n";
        } else {
            resultCode += "idiv %" + registersIdentMap.find(argReg)->second
                          + "\n";
        }
    } else {
        resultCode += op + " %" + registersIdentMap.find(argReg)->second + ", %"
                      + registersIdentMap.find(resReg)->second
                      + "\n";
    }

    if (op == "mod") {
        resReg = 3;
        if (registerAllocationMap.find(res)->second == -1) {
            resultCode += "movq %" + registersIdentMap.find(3)->second + ", %"
                          + registersIdentMap.find(registerAllocationMap.find(res)->second)->second + "\n";
            resReg = registerAllocationMap.find(res)->second;
        }
    }
    if (op == "idiv") {
        if (registerAllocationMap.find(res)->second == -1) {
            resultCode += "movq %" + registersIdentMap.find(0)->second + ", %"
                          + registersIdentMap.find(registerAllocationMap.find(res)->second)->second + "\n";
            resReg = registerAllocationMap.find(res)->second;
        }
    }
    if (op == "cmp") {
        Ident trueLabel = symbolsTable->getNewSymbol();
        Ident endLabel = symbolsTable->getNewSymbol();
        if (registerAllocationMap.find(res)->second == -1) {
            resReg = getFreeRegister(-1, -1);
        } else {
            resReg = registerAllocationMap.find(res)->second;
        }
        resultCode += relOp + " " + trueLabel + "\n";
        resultCode += "movq $0, %" + registersIdentMap.find(resReg)->second + "\n";
        resultCode += "jmp " + endLabel + "\n";
        resultCode += trueLabel + ":\n";
        resultCode += "movq $1, %" + registersIdentMap.find(resReg)->second + "\n";
        resultCode += endLabel + ":\n";

    }
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    values.find(res)->second.isSaved = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.isValue = false;
    values.find(res)->second.type = "int";
    saveValue(res);
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
    }

    if (registerAllocationMap.find(arg1)->second != -1) {
        saveValue(arg1);
    }

    if (values.find(arg1)->second.isValue) {
        if (op == "idiv" || op == "mod") {
            resultCode += "movq $" + std::to_string(values.find(arg1)->second.value) + ", %" +
                          registersIdentMap.find(resReg)->second
                          + "\n";
            resultCode += "cqo\n";
        } else {
            resReg = getFreeRegister(-1, -1);
            resultCode += "movq $" + std::to_string(values.find(arg1)->second.value) + ", %" +
                          registersIdentMap.find(resReg)->second
                          + "\n";
        }
    } else {
        if (registerAllocationMap.find(res)->second != -1 && resReg == -1) {
            resReg = registerAllocationMap.find(res)->second;
        }
        for (Location location : values.find(arg1)->second.locations) {
            argReg = location.reg;
            if (registers.find(location.reg)->second.contents.size() == 1 && resReg == -1 &&
                values.find(arg1)->second.isSaved) {
                resReg = location.reg;
                break;
            }
        }
        if (registerAllocationMap.find(arg1)->second != -1) {
            saveValue(arg1);
            argReg = registerAllocationMap.find(arg1)->second;
        }

        if (op == "idiv" || op == "mod") {
            if (argReg != -1) {
                resultCode += "movq %" + registersIdentMap.find(argReg)->second + ", %"
                              + registersIdentMap.find(0)->second + "\n";
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg1)->second;
                resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + ", %"
                              + registersIdentMap.find(0)->second + "\n";
            }
            resultCode += "cqo\n";
        }
        if (resReg == -1) {
            if (argReg != -1) {
                resReg = getFreeRegister(argReg, -1);
                resultCode += "movq %" + registersIdentMap.find(argReg)->second + ", %"
                              + registersIdentMap.find(resReg)->second + "\n";
            } else {
                resReg = getFreeRegister(-1, -1);
                int varLocation = actualBasicBlock->memoryMap.find(arg1)->second;
                resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + ", %"
                              + registersIdentMap.find(resReg)->second + "\n";
            }
        } else {
            if (resReg != argReg) {
                if (argReg != -1) {
                    resultCode += "movq %" + registersIdentMap.find(argReg)->second + ", %"
                                  + registersIdentMap.find(resReg)->second + "\n";
                } else {
                    int varLocation = actualBasicBlock->memoryMap.find(arg1)->second;
                    resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + ", %"
                                  + registersIdentMap.find(resReg)->second + "\n";
                }
            }
        }
    }

    argReg = getFreeRegister(resReg, 3);
    resultCode += "movq $" + std::to_string(val2) + ", %" + registersIdentMap.find(argReg)->second
                  + "\n";
    if (op == "idiv" || op == "mod") {
        if (op == "idiv") {
            resultCode += op + " %" + registersIdentMap.find(argReg)->second
                          + "\n";
        } else {
            resultCode += "idiv %" + registersIdentMap.find(argReg)->second
                          + "\n";
        }
    } else {
        resultCode += op + " %" + registersIdentMap.find(argReg)->second + ", %"
                      + registersIdentMap.find(resReg)->second + "\n";
    }
    if (op == "mod") {
        resReg = 3;
        if (registerAllocationMap.find(res)->second != -1) {
            resReg = registerAllocationMap.find(res)->second;
            resultCode += "movq %" + registersIdentMap.find(3)->second + ", %"
                          + registersIdentMap.find(resReg)->second + "\n";
        }
    }
    if (op == "idiv") {
        if (registerAllocationMap.find(res)->second != -1) {
            resReg = registerAllocationMap.find(res)->second;
            resultCode += "movq %" + registersIdentMap.find(0)->second + ", %"
                          + registersIdentMap.find(resReg)->second + "\n";
        }
    }
    if (op == "cmp") {
        Ident trueLabel = symbolsTable->getNewSymbol();
        Ident endLabel = symbolsTable->getNewSymbol();
        if (registerAllocationMap.find(res)->second != -1) {
            resReg = registerAllocationMap.find(res)->second;
        } else {
            resReg = getFreeRegister(-1, -1);
        }
        resultCode += relOp + " " + trueLabel + "\n";
        resultCode += "movq $0, %" + registersIdentMap.find(resReg)->second + "\n";
        resultCode += "jmp " + endLabel + "\n";
        resultCode += trueLabel + ":\n";
        resultCode += "movq $1, %" + registersIdentMap.find(resReg)->second + "\n";
        resultCode += endLabel + ":\n";

    }
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    values.find(res)->second.isSaved = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.isValue = false;
    values.find(res)->second.type = "int";
    saveValue(res);
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
        resultCode += "movq $" + std::to_string(val1) + ", %" + registersIdentMap.find(resReg)->second
                      + "\n";
        resultCode += "cqo\n";
    } else {
        if (registerAllocationMap.find(res)->second != -1 &&
            registerAllocationMap.find(res)->second != registerAllocationMap.find(arg2)->second) {
            resReg = registerAllocationMap.find(res)->second;
        } else {
            resReg = getFreeRegister(-1, -1);
        }
        resultCode += "movq $" + std::to_string(val1) + ", %" + registersIdentMap.find(resReg)->second
                      + "\n";
    }
    if (registerAllocationMap.find(arg2)->second != -1) {
        saveValue(arg2);
    }

    if (values.find(arg2)->second.isValue) {
        if (op == "idiv" || op == "mod") {
            argReg = getFreeRegister(0, 3);
            resultCode += "movq $" + std::to_string(values.find(arg2)->second.value) + ", %" +
                          registersIdentMap.find(argReg)->second
                          + "\n";
            resultCode += "idiv %" + registersIdentMap.find(argReg)->second + "\n";
        } else {
            argReg = getFreeRegister(resReg, -1);
            resultCode += "movq $" + std::to_string(values.find(arg2)->second.value) + ", %" +
                          registersIdentMap.find(argReg)->second
                          + "\n";
            resultCode += op + " %" + registersIdentMap.find(argReg)->second + ", %"
                          + registersIdentMap.find(resReg)->second + "\n";
        }
    } else {
        argReg = -1;
        for (Location location : values.find(arg2)->second.locations) {
            argReg = location.reg;
        }
        if (registerAllocationMap.find(arg2)->second != -1) {
            argReg = registerAllocationMap.find(arg2)->second;
        }
        if (argReg != -1) {
            if (op == "idiv" || op == "mod") {
                resultCode += "idiv %" + registersIdentMap.find(argReg)->second + "\n";
            } else {
                resultCode += op + " %" + registersIdentMap.find(argReg)->second + ", %"
                              + registersIdentMap.find(resReg)->second + "\n";
            }
        } else {
            if (op == "idiv" || op == "mod") {
                resultCode += "idiv %" + registersIdentMap.find(argReg)->second + "\n";
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg2)->second;
                resultCode += op + " " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + ", %"
                              + registersIdentMap.find(resReg)->second + "\n";
            }
        }
    }
    if (op == "mod") {
        resReg = 3;
        if (registerAllocationMap.find(res)->second != -1) {
            resReg = registerAllocationMap.find(res)->second;
            resultCode += "movq %" + registersIdentMap.find(3)->second + ", %"
                          + registersIdentMap.find(resReg)->second + "\n";
        }
    }
    if (op == "idiv") {
        if (registerAllocationMap.find(res)->second != -1) {
            resReg = registerAllocationMap.find(res)->second;
            resultCode += "movq %" + registersIdentMap.find(0)->second + ", %"
                          + registersIdentMap.find(resReg)->second + "\n";
        }
    }
    if (op == "cmp") {
        Ident trueLabel = symbolsTable->getNewSymbol();
        Ident endLabel = symbolsTable->getNewSymbol();
        if (registerAllocationMap.find(res)->second != -1) {
            resReg = registerAllocationMap.find(res)->second;
        } else {
            resReg = getFreeRegister(-1, -1);
        }
        resultCode += relOp + " " + trueLabel + "\n";
        resultCode += "movq $0, %" + registersIdentMap.find(resReg)->second + "\n";
        resultCode += "jmp " + endLabel + "\n";
        resultCode += trueLabel + ":\n";
        resultCode += "movq $1, %" + registersIdentMap.find(resReg)->second + "\n";
        resultCode += endLabel + ":\n";

    }
    if (registerAllocationMap.find(res)->second != -1 && resReg != registerAllocationMap.find(res)->second) {
        resultCode += "movq %" + registersIdentMap.find(resReg)->second + ", %"
                      + registersIdentMap.find(registerAllocationMap.find(res)->second)->second + "\n";
        resReg = registerAllocationMap.find(res)->second;
    }

    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    values.find(res)->second.isSaved = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.isValue = false;
    values.find(res)->second.type = "int";
    saveValue(res);
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
    if (registerAllocationMap.find(arg1)->second != -1) {
        saveValue(arg1);
    }
    if (registerAllocationMap.find(arg2)->second != -1) {
        saveValue(arg2);
    }
    if (op == "idiv" || op == "mod") {
        if (!registers.find(3)->second.isFree) {
            spillRegister(3);
        }
        if (!registers.find(0)->second.isFree) {
            spillRegister(0);
        }
        resReg = 0;
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
            resultCode += "movq $" + std::to_string(values.find(arg1)->second.value) + ", %" +
                          registersIdentMap.find(resReg)->second
                          + "\n";
            resultCode += "cqo\n";
        } else {
            resReg = getFreeRegister(-1, -1);
            resultCode += "movq $" + std::to_string(values.find(arg1)->second.value) + ", %" +
                          registersIdentMap.find(resReg)->second
                          + "\n";
        }
    } else {
        if (registerAllocationMap.find(res)->second != -1 && resReg == -1) {
            resReg = registerAllocationMap.find(res)->second;
        }
        for (Location location : values.find(arg1)->second.locations) {
            argReg = location.reg;
            if (registers.find(location.reg)->second.contents.size() == 1 && values.find(arg1)->second.isSaved &&
                resReg == -1) {
                resReg = location.reg;
                break;
            }
        }
        if (registerAllocationMap.find(arg1)->second != -1) {
            argReg = registerAllocationMap.find(arg1)->second;
        }

        if (op == "idiv" || op == "mod") {
            if (argReg != -1) {
                resultCode += "movq %" + registersIdentMap.find(argReg)->second + ", %"
                              + registersIdentMap.find(resReg)->second + "\n";
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg1)->second;
                resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + ", %"
                              + registersIdentMap.find(resReg)->second + "\n";
            }
            resultCode += "cqo\n";
        } else {
            if (resReg == -1 || resReg == registerAllocationMap.find(arg2)->second) {
                resReg = getFreeRegister(-1, -1);
                if (argReg != -1) {
                    resultCode += "movq %" + registersIdentMap.find(argReg)->second + ", %"
                                  + registersIdentMap.find(resReg)->second + "\n";
                } else {
                    int varLocation = actualBasicBlock->memoryMap.find(arg1)->second;
                    resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + ", %"
                                  + registersIdentMap.find(resReg)->second + "\n";
                }

            } else {
                if (argReg != -1) {
                    resultCode += "movq %" + registersIdentMap.find(argReg)->second + ", %"
                                  + registersIdentMap.find(resReg)->second + "\n";
                } else {
                    int varLocation = actualBasicBlock->memoryMap.find(arg1)->second;
                    resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + ", %"
                                  + registersIdentMap.find(resReg)->second + "\n";
                }
            }
        }
    }

    if (values.find(arg2)->second.isValue) {
        if (op == "idiv" || op == "mod") {
            argReg = getFreeRegister(resReg, 3);
            resultCode += "movq $" + std::to_string(values.find(arg2)->second.value) + ", %" +
                          registersIdentMap.find(argReg)->second
                          + "\n";
            resultCode += "idiv %" + registersIdentMap.find(argReg)->second + "\n";
        } else {
            argReg = getFreeRegister(resReg, -1);
            resultCode += "movq $" + std::to_string(values.find(arg2)->second.value) + ", %" +
                          registersIdentMap.find(argReg)->second
                          + "\n";
            resultCode += op + " %" + registersIdentMap.find(argReg)->second + ", %"
                          + registersIdentMap.find(resReg)->second + "\n";
        }
    } else {
        argReg = -1;
        for (Location location : values.find(arg2)->second.locations) {
            argReg = location.reg;
        }
        if (registerAllocationMap.find(arg2)->second != -1) {
            argReg = registerAllocationMap.find(arg2)->second;
        }
        if (op == "idiv" || op == "mod") {
            if (argReg != -1) {
                resultCode += "idiv %" + registersIdentMap.find(argReg)->second + "\n";
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg2)->second;
                resultCode += "idivq " + std::to_string((varLocation + 1) * -8) + "(%rbp)" + "\n";
            }
        } else {
            if (argReg != -1) {
                resultCode += op + " %" + registersIdentMap.find(argReg)->second + ", %"
                              + registersIdentMap.find(resReg)->second + "\n";
            } else {
                int varLocation = actualBasicBlock->memoryMap.find(arg2)->second;
                resultCode += op + " " + std::to_string((varLocation + 1) * -8) + "(%rbp), %"
                              + registersIdentMap.find(resReg)->second
                              + "\n";
            }
        }
    }
    if (op == "mod") {
        resReg = 3;
        if (registerAllocationMap.find(res)->second != -1) {
            resReg = registerAllocationMap.find(res)->second;
            resultCode += "movq %" + registersIdentMap.find(3)->second + ", %"
                          + registersIdentMap.find(resReg)->second + "\n";
        }
    }
    if (op == "idiv") {
        if (registerAllocationMap.find(res)->second != -1) {
            resReg = registerAllocationMap.find(res)->second;
            resultCode += "movq %" + registersIdentMap.find(0)->second + ", %"
                          + registersIdentMap.find(resReg)->second + "\n";
        }
    }
    if (op == "cmp") {
        Ident trueLabel = symbolsTable->getNewSymbol();
        Ident endLabel = symbolsTable->getNewSymbol();
        if (registerAllocationMap.find(res)->second != -1) {
            resReg = registerAllocationMap.find(res)->second;
        } else {
            resReg = getFreeRegister(-1, -1);
        }
        resultCode += relOp + " " + trueLabel + "\n";
        resultCode += "movq $0, %" + registersIdentMap.find(resReg)->second + "\n";
        resultCode += "jmp " + endLabel + "\n";
        resultCode += trueLabel + ":\n";
        resultCode += "movq $1, %" + registersIdentMap.find(resReg)->second + "\n";
        resultCode += endLabel + ":\n";

    }
    if (registerAllocationMap.find(res)->second != -1 && registerAllocationMap.find(res)->second != resReg) {
        resultCode += "movq %" + registersIdentMap.find(resReg)->second + ", %"
                      + registersIdentMap.find(registerAllocationMap.find(res)->second)->second + "\n";
        resReg = registerAllocationMap.find(res)->second;
    }
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    values.find(res)->second.isSaved = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.isValue = false;
    values.find(res)->second.type = "int";
    saveValue(res);
}

void Allocator::genRet(Ident retVal) {
    if (registerAllocationMap.find(retVal)->second != -1) {
        saveValue(retVal);
    }
    if (values.find(retVal)->second.isValue) {
        resultCode += "movl $" + std::to_string(values.find(retVal)->second.value) + ", %eax\n";
    } else {
        int retReg = -1;
        for (Location location : values.find(retVal)->second.locations) {
            retReg = location.reg;
        }
        if (registerAllocationMap.find(retVal)->second != -1) {
            retReg = registerAllocationMap.find(retVal)->second;
        }
        if (retReg == -1) {
            int varLocation = actualBasicBlock->memoryMap.find(retVal)->second;
            resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp), %rax\n";
        } else {
            resultCode += "movq %" + registersIdentMap.find(retReg)->second + ", %rax\n";
        }
    }
    if (actualBasicBlock->funIdent != "main") {

        resultCode += "popq %r15\n";
        resultCode += "popq %r14\n";
        resultCode += "popq %r13\n";
        resultCode += "popq %r12\n";
        resultCode += "popq %rbx\n";
    }
    resultCode += "movq %rbp, %rsp\n";
    resultCode += "popq %rbp\n";
    resultCode += "ret\n";
}

void Allocator::genRet(int retVal) {
    if (actualBasicBlock->funIdent != "main") {
        resultCode += "popq %r15\n";
        resultCode += "popq %r14\n";
        resultCode += "popq %r13\n";
        resultCode += "popq %r12\n";
        resultCode += "popq %rbx\n";
    }
    resultCode += "movq %rbp, %rsp\n";
    resultCode += "popq %rbp\n";
    resultCode += "movl $" + std::to_string(retVal) + ", %eax\nret\n";
}

void Allocator::genIf(Ident cond, Ident label1, Ident label2, std::map<Ident, LivenessInfo> liveVariables,
                      std::map<Ident, int> actualBlockMemMap, std::map<Ident, int> nextBlockMemMapLabel1,
                      std::map<Ident, int> nextBlockMemMapLabel2) {
    if (registerAllocationMap.find(cond)->second != -1) {
        saveValue(cond);
    }
    if (values.find(cond)->second.isValue) {
        if (values.find(cond)->second.value == 0) {
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel2, false);
            resultCode += "jmp " + label2 + "\n";
        } else {
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel1, false);
            resultCode += "jmp " + label1 + "\n";
        }
    } else {
        int retReg = -1;
        for (Location location : values.find(cond)->second.locations) {
            retReg = location.reg;
            if (registers.find(location.reg)->second.contents.size() == 1 && values.find(cond)->second.isSaved) {
                retReg = location.reg;
                break;
            }
        }
        if (registerAllocationMap.find(cond)->second != -1) {
            retReg = registerAllocationMap.find(cond)->second;
        }
        Ident l1, l2;
        l1 = symbolsTable->getNewSymbol();
        l2 = symbolsTable->getNewSymbol();
        if (retReg == -1) {
            int varLocation = actualBasicBlock->memoryMap.find(cond)->second;
            resultCode += "cmp $0, " + std::to_string((varLocation + 1) * -8) + "(%rbp)\n";
            resultCode += "je " + l2 + "\n";
            resultCode += "jne " + l1 + "\n";
            resultCode += l2 + ": \n";
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel2, false);
            resultCode += "jmp " + label2 + "\n";
            resultCode += l1 + ":\n";
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel1, false);
            resultCode += "jmp " + label1 + "\n";
        } else {
            resultCode += "cmp $0, %" + registersIdentMap.find(retReg)->second + "\n";
            resultCode += "je " + l2 + "\n";
            resultCode += "jne " + l1 + "\n";
            resultCode += l2 + ": \n";
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel2, false);
            resultCode += "jmp " + label2 + "\n";
            resultCode += l1 + ":\n";
            moveLocalVariables(liveVariables, actualBlockMemMap, nextBlockMemMapLabel1, false);
            resultCode += "jmp " + label1 + "\n";
        }
    }
}

void Allocator::writeLiveValues() {
    for (auto it = values.begin(); it != values.end(); it++) {
        if (it->second.isValue) {
            if (registerAllocationMap.find(it->first)->second != -1) {
                saveValue(it->first);
            } else {
                int reg = getFreeRegister(-1, -1);
                resultCode +=
                        "movq $" + std::to_string(it->second.value) + ", %" + registersIdentMap.find(reg)->second +
                        "\n";
                int varLocation = actualBasicBlock->memoryMap.find(it->second.ident)->second;
                resultCode +=
                        "movq  $" + std::to_string(it->second.value) + ", " + std::to_string((varLocation + 1) * -8) +
                        "(%rbp)\n";
            }
            it->second.isValue = false;
            it->second.isSaved = true;
        } else if (!it->second.isSaved) {
            if (registerAllocationMap.find(it->first)->second != -1) {
                saveValue(it->first);
            } else {
                int oldReg = -1;
                for (auto loc : it->second.locations) {
                    oldReg = loc.reg;
                    break;
                }
                int varLocation = actualBasicBlock->memoryMap.find(it->second.ident)->second;
                resultCode += "movq  %" + registersIdentMap.find(oldReg)->second + ", " +
                              std::to_string((varLocation + 1) * -8)
                              + "(%rbp)\n";
            }
            it->second.isValue = false;
            it->second.isSaved = true;
        }
    }
}

void Allocator::moveLocalVariables(std::map<Ident, LivenessInfo> liveVariables, std::map<Ident, int> actualBlockMemMap,
                                   std::map<Ident, int> nextBlockMemMap, bool isAfterCall) {
    int movRegister = -1;
    int actualVarLocation, nextVarLocation;
    std::set<Ident> movedValues;
    bool isXchg;
    for (auto live : liveVariables) {
        if (registerAllocationMap.find(live.first)->second == -1) {
            isXchg = false;
            if (nextBlockMemMap.find(live.second.ident) != nextBlockMemMap.end()) {
                nextVarLocation = nextBlockMemMap.find(live.first)->second;
                actualVarLocation = actualBlockMemMap.find(live.first)->second;
                if (nextVarLocation != actualVarLocation) {
                    for (auto it : actualBlockMemMap) {
                        if (it.second == nextVarLocation && liveVariables.find(it.first) != liveVariables.end() &&
                            movedValues.find(it.first) == movedValues.end()) {
                            if (movRegister == -1) {
                                movRegister = getFreeRegister(0, -1);
                            }
                            resultCode += "movq " + std::to_string((nextVarLocation + 1) * -8) + "(%rbp), %"
                                          + registersIdentMap.find(movRegister)->second + "\n";
                            resultCode += "xchg %" + registersIdentMap.find(movRegister)->second + ", "
                                          + std::to_string((actualVarLocation + 1) * -8) + "(%rbp)\n";
                            resultCode += "movq %" + registersIdentMap.find(movRegister)->second + ", "
                                          + std::to_string((nextVarLocation + 1) * -8) + "(%rbp)\n";
                            actualBlockMemMap.erase(it.first);
                            actualBlockMemMap.emplace(it.first, actualVarLocation);
                            movedValues.insert(live.first);
                            isXchg = true;
                            break;
                        }
                    }
                }
                if (isXchg && !values.find(live.second.ident)->second.isValue &&
                    values.find(live.second.ident)->second.isSaved) {
                    continue;
                }
                if (values.find(live.second.ident)->second.isValue) {
                    resultCode += "movq $" + std::to_string(values.find(live.second.ident)->second.value) + ", "
                                  + std::to_string((nextVarLocation + 1) * -8) + "(%rbp)\n";
                } else if (!values.find(live.second.ident)->second.isValue &&
                           !values.find(live.second.ident)->second.locations.empty() &&
                           !(isAfterCall && values.find(live.second.ident)->second.locations.begin()->reg == 0)) {
                    resultCode += "movq %" + registersIdentMap.find(
                            values.find(live.second.ident)->second.locations.begin()->reg)->second + ", "
                                  + std::to_string((nextVarLocation + 1) * -8) + "(%rbp)\n";
                } else {
                    actualVarLocation = actualBlockMemMap.find(live.first)->second;
                    if (actualVarLocation != nextVarLocation) {
                        if (movRegister == -1) {
                            movRegister = getFreeRegister(0, -1);
                        }
                        resultCode += "movq " + std::to_string((actualVarLocation + 1) * -8) + "(%rbp), %"
                                      + registersIdentMap.find(movRegister)->second + "\n";
                        resultCode += "movq %" + registersIdentMap.find(movRegister)->second + ", "
                                      + std::to_string((nextVarLocation + 1) * -8) + "(%rbp)\n";
                    }
                }
                movedValues.insert(live.first);
            }
        } else {
            saveValue(live.first);
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

void Allocator::genParam(int paramNum, int val) {
    std::vector<int> paramsReg = {5, 4, 3, 2, 6, 7};
    if (paramNum + 1 < paramsReg.size()) {
        if (!registers.find(paramsReg[paramNum + 1])->second.isFree) {
            spillRegister(paramsReg[paramNum + 1]);
        }
        resultCode +=
                "movq $" + std::to_string(val) + ", %" + registersIdentMap.find(paramsReg[paramNum + 1])->second + "\n";
    } else {
        resultCode += "pushq $" + std::to_string(val) + "\n";
    }
}

void Allocator::genParam(int paramNum, Ident val) {
    std::vector<int> paramsReg = {5, 4, 3, 2, 6, 7};
    if (paramNum + 1 < paramsReg.size()) {
        if (!registers.find(paramsReg[paramNum + 1])->second.isFree) {
            spillRegister(paramsReg[paramNum + 1]);
        }
        if (registerAllocationMap.find(val)->second != -1) {
            saveValue(val);
            resultCode += "movq %" + registersIdentMap.find(registerAllocationMap.find(val)->second)->second
                          + ", %" + registersIdentMap.find(paramsReg[paramNum + 1])->second + "\n";
        } else if (values.find(val)->second.isValue) {
            resultCode += "movq $" + std::to_string(values.find(val)->second.value) + ", %"
                          + registersIdentMap.find(paramsReg[paramNum + 1])->second + "\n";
        } else if (!values.find(val)->second.isValue && !values.find(val)->second.locations.empty()) {
            resultCode += "movq %" + registersIdentMap.find(values.find(val)->second.locations.begin()->reg)->second
                          + ", %" + registersIdentMap.find(paramsReg[paramNum + 1])->second + "\n";
        } else {
            int varLocation = actualBasicBlock->memoryMap.find(val)->second;
            resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp), %"
                          + registersIdentMap.find(paramsReg[paramNum + 1])->second + "\n";
        }
    } else {
        if (registerAllocationMap.find(val)->second != -1) {
            saveValue(val);
            resultCode += "pushq %" + registersIdentMap.find(registerAllocationMap.find(val)->second)->second + "\n";
        } else if (values.find(val)->second.isValue) {
            resultCode += "pushq $" + std::to_string(values.find(val)->second.value) + "\n";
        } else if (!values.find(val)->second.isValue && !values.find(val)->second.locations.empty()) {
            resultCode += "pushq %" + registersIdentMap.find(values.find(val)->second.locations.begin()->reg)->second
                          + "\n";
        } else {
            int varLocation = actualBasicBlock->memoryMap.find(val)->second;
            resultCode += "pushq " + std::to_string((varLocation + 1) * -8) + "(%rbp)\n";
        }
    }
}

void Allocator::genRetrieve(Ident ident) {
    values.find(ident)->second.isSaved = false;
    Location location;
    location.reg = 0;
    values.find(ident)->second.locations.insert(location);
    registers.find(0)->second.isFree = false;
    registers.find(0)->second.contents.clear();
    registers.find(0)->second.contents.insert(ident);
    saveValue(ident);
    spillRegister(0);
}

void Allocator::initFunArgs(std::vector<Ident> argIdents, std::map<Ident, LivenessInfo> liveVariables,
                            std::map<Ident, int> actualBlockMemMap) {
    std::vector<int> paramsReg = {5, 4, 3, 2, 6, 7};
    for (int i = 0; i < argIdents.size(); i++) {
        if (liveVariables.find(argIdents[i]) != liveVariables.end()) {
            if (argIdents.size() - i <= paramsReg.size()) {
                values.find(argIdents[i])->second.isSaved = false;
                Location location;
                location.reg = paramsReg[argIdents.size() - i - 1];
                values.find(argIdents[i])->second.locations.insert(location);
            } else {
                if (registerAllocationMap.find(argIdents[i])->second != -1) {
                    resultCode += "movq " + std::to_string(-((argIdents.size() - paramsReg.size()) + 1 + i) * -8)
                                  + "(%rbp), %"
                                  + registersIdentMap.find(registerAllocationMap.find(argIdents[i])->second)->second
                                  + "\n";
                } else {
                    actualBlockMemMap.erase(argIdents[i]);
                    actualBlockMemMap.emplace(argIdents[i], -(argIdents.size() - paramsReg.size()) + 1 + i);
                }
            }
        }
    }
}


void Allocator::addString(Ident res, Ident s1, Ident s2) {
    int resReg;
    for (Location location : values.find(res)->second.locations) {
        registers.find(location.reg)->second.contents.erase(res);
        if (registers.find(location.reg)->second.contents.empty()) {
            registers.find(location.reg)->second.isFree = true;
        }
    }
    writeLiveValues();
    if (!registers.find(5)->second.isFree) {
        spillRegister(5);
    }
    if (registerAllocationMap.find(s1)->second != -1) {
        saveValue(s1);
        resultCode += "movq %" + registersIdentMap.find(registerAllocationMap.find(s1)->second)->second
                      + ", %rdi\n";
    } else if (!values.find(s1)->second.isValue && !values.find(s1)->second.locations.empty()) {
        resultCode += "movq %" + registersIdentMap.find(values.find(s1)->second.locations.begin()->reg)->second
                      + ", %rdi\n";
    } else {
        int varLocation = actualBasicBlock->memoryMap.find(s1)->second;
        resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp), %rdi\n";
    }
    if (!registers.find(4)->second.isFree) {
        spillRegister(4);
    }
    if (registerAllocationMap.find(s2)->second != -1) {
        saveValue(s2);
        resultCode += "movq %" + registersIdentMap.find(registerAllocationMap.find(s2)->second)->second
                      + ", %rsi\n";
    } else if (!values.find(s2)->second.isValue && !values.find(s2)->second.locations.empty()) {
        resultCode += "movq %" + registersIdentMap.find(values.find(s2)->second.locations.begin()->reg)->second
                      + ", %rsi\n";
    } else {
        int varLocation = actualBasicBlock->memoryMap.find(s2)->second;
        resultCode += "movq " + std::to_string((varLocation + 1) * -8) + "(%rbp), %rsi\n";
    }
    if (!registers.find(0)->second.isFree) {
        spillRegister(0);
    }
    if ((stackAlignment->stackAlignment + 8) % 16 != 0) {
        resultCode += "subq $8, %rsp\n";
        stackAlignment->isStackAlignedBeforeCall = true;
    }
    resultCode += "pushq %r11\n";
    resultCode += "call _concat\n";
    resultCode += "popq %r11\n";
    if (stackAlignment->isStackAlignedBeforeCall) {
        resultCode += "add $8, %rsp\n";
        stackAlignment->isStackAlignedBeforeCall = false;
    }

    resReg = 0;
    registers.find(resReg)->second.isFree = false;
    registers.find(resReg)->second.contents.insert(res);
    values.find(res)->second.isValue = false;
    values.find(res)->second.isSaved = false;
    Location location1;
    location1.reg = resReg;
    values.find(res)->second.locations.insert(location1);
    values.find(res)->second.type = "string";
    saveValue(res);
    saveRax();
    clearRegistersInfo();
}

void Allocator::saveRax() {
    if (!registers.find(0)->second.isFree) {
        spillRegister(0);
    }
}

void Allocator::clearRegistersInfo() {
    for (auto it = registers.begin(); it != registers.end(); it++) {
        if (it->first < 9) {
            it->second.contents.clear();
            it->second.isFree = true;
        }
    }
    for (auto it = values.begin(); it != values.end(); it++) {
        it->second.locations.clear();
        if (registerAllocationMap.find(it->first)->second != -1) {
            Location location;
            location.reg = registerAllocationMap.find(it->first)->second;
            it->second.locations.insert(location);
        }
    }
}

void Allocator::saveValue(Ident ident) {
    int resReg = registerAllocationMap.find(ident)->second;
    if (!values.find(ident)->second.isSaved && resReg != -1) {
        if (values.find(ident)->second.isValue) {
            resultCode += "movq $" + std::to_string(values.find(ident)->second.value) + ", %" +
                          registersIdentMap.find(resReg)->second
                          + "\n";
            values.find(ident)->second.isValue = false;
        } else {
            if (registersIdentMap.find(values.find(ident)->second.locations.begin()->reg)->second !=
                registersIdentMap.find(resReg)->second) {
                resultCode += "movq %"
                              + registersIdentMap.find(values.find(ident)->second.locations.begin()->reg)->second
                              + ", %" + registersIdentMap.find(resReg)->second + "\n";
            }
        }
        values.find(ident)->second.isSaved = true;
    }
}

void Allocator::popFunArgs(int paramNum) {
    for (int i = 0; i < paramNum - 6; i++) {
        resultCode += "add $8, %rsp\n";
    }
}


Location::Location() {}

bool Location::operator<(const Location &other) const {
    return other.reg > this->reg;
}
