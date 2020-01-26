//
// Created by szymon on 06.01.2020.
//

#include <iostream>
#include "CodeGenerator.h"
#include "GlobalAllocator.h"

void CodeGenerator::visitQuadAss1(QuadAss1 *q) {
    if (q->arg.isValue) {
        allocator->genAss1(q->res, q->arg.value, q->op);
    } else {
        allocator->genAss1(q->res, q->arg.var, q->op);
    }
}

void CodeGenerator::visitQuadAss2(QuadAss2 *q) {
    if (q->arg1.isValue && q->arg2.isValue) {
        allocator->genAss2(q->res, q->arg1.value, q->arg2.value, q->op);
    }
    if (!q->arg1.isValue && q->arg2.isValue) {
        allocator->genAss2(q->res, q->arg1.var, q->arg2.value, q->op);
    }
    if (q->arg1.isValue && !q->arg2.isValue) {
        allocator->genAss2(q->res, q->arg1.value, q->arg2.var, q->op);
    }
    if (!q->arg1.isValue && !q->arg2.isValue) {
        if (q->arg1.type == "string") {
            allocator->addString(q->res, q->arg1.var, q->arg2.var);
        } else {
            allocator->genAss2(q->res, q->arg1.var, q->arg2.var, q->op);
        }
    }
}

void CodeGenerator::visitQuadCopy(QuadCopy *q) {
    if (q->arg.isValue) {
        if (q->arg.type == "string") {
            allocator->copyString(q->res, q->arg.stringValue);
        } else {
            allocator->copy(q->res, q->arg.value);
        }
    } else {
        allocator->copy(q->res, q->arg.var);
    }
}

void CodeGenerator::visitQuadJmp(QuadJmp *q) {
    isBlockEndedWithJump = true;
    // allocator->writeLiveValues();
    allocator->moveLocalVariables(*actualBasicBlock->liveness.begin(), actualBasicBlock->memoryMap,
                                  (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                          q->label)->second]->memoryMap, false);
    std::cout << q->jmpOp << " " << q->label << "\n";
}

void CodeGenerator::visitQuadLabel(QuadLabel *q) {
    std::cout << q->label << ":\n";
}

void CodeGenerator::visitQuadIf(QuadIf *q) {
    isBlockEndedWithJump = true;
    if (q->cond.isValue) {
        if (q->cond.value == 0) {
            //allocator->writeLiveValues();
            allocator->moveLocalVariables(*actualBasicBlock->liveness.begin(), actualBasicBlock->memoryMap,
                                          (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                                  q->label2)->second]->memoryMap, false);
            std::cout << "jmp " << q->label2 << "\n";
        } else {
            //allocator->writeLiveValues();
            allocator->moveLocalVariables(*actualBasicBlock->liveness.begin(), actualBasicBlock->memoryMap,
                                          (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                                  q->label1)->second]->memoryMap, false);
            std::cout << "jmp " << q->label1 << "\n";
        }
    } else {
        allocator->genIf(q->cond.var, q->label1, q->label2, *actualBasicBlock->liveness.begin(),
                         actualBasicBlock->memoryMap,
                         (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                 q->label1)->second]->memoryMap,
                         (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                 q->label2)->second]->memoryMap);
    }
}

void CodeGenerator::visitQuadParam(QuadParam *q) {
    // isBlockEndedWithJump = true;
    if (paramNum == -1 && !actualBasicBlock->successors.empty()) {
        allocator->writeLiveValues();
    }
    if (q->arg.isValue) {
        allocator->genParam(paramNum, q->arg.value);
    } else {
        allocator->genParam(paramNum, q->arg.var);
    }
    paramNum++;
}

void CodeGenerator::visitQuadCall(QuadCall *q) {
    // isBlockEndedWithJump = true;
    if (paramNum == -1 && !actualBasicBlock->successors.empty()) {
        allocator->writeLiveValues();
    }
    paramNum = -1;
    std::cout << "pushq %r11\n";
    std::cout << "call " << q->label << "\n";
    std::cout << "popq %r11\n";
    if (functionHeaders->getHeader(q->label).returnType == "void") {
        allocator->clearRegistersInfo();
    }
    isAfterCall = true;
}

void CodeGenerator::visitQuadReturn(QuadReturn *q) {
    if (q->ret.isValue) {
        allocator->genRet(q->ret.value);
    } else {
        allocator->genRet(q->ret.var);
    }
}

void CodeGenerator::visitQuadReturnNoVal(QuadReturnNoVal *q) {
    if (actualBasicBlock->funIdent != "main") {
        popCallPreservedRegisters();
    }
    std::cout << "movq %rbp, %rsp\n";
    std::cout << "popq %rbp\n";
    std::cout << "ret\n";
}

void CodeGenerator::visitQuadRetrieve(QuadRetrieve *q) {
    allocator->genRetrieve(q->res);
    allocator->saveRax();
    allocator->clearRegistersInfo();
}

void CodeGenerator::visitQuadFunBegin(QuadFunBegin *q) {
    GlobalAllocator globalAllocator;
    registerAllocationMap = globalAllocator.allocateRegisters(q->ident, controlFlowGraph);
    allocator->registerAllocationMap = registerAllocationMap;
    // printRegisterAllocation();
    int numOfVariables = getNumOfLocalVariables(q->ident);
    std::cout << q->ident << ":" << "\n";
    std::cout << "pushq %rbp\n";
    std::cout << "movq %rsp, %rbp\n";
    std::cout << "subq $" << numOfVariables * 8 << ", %rsp\n";
    if (q->ident != "main") {
        pushCallPreservedRegisters();
    }
    allocator->initFunArgs(functionHeaders->getHeader(q->ident).newArgSymbols, *actualBasicBlock->liveness.rbegin(),
                           actualBasicBlock->memoryMap);
}

void CodeGenerator::generateCode() {
    allocator->initRegisters();
    allocator->initRegistersIdentMap();
    std::cout << ".extern printInt\n"
                 ".extern printString\n"
                 ".extern readInt\n"
                 ".extern readString\n"
                 ".extern concat\n"
                 ".text\n";
    for (auto stringPair: stringValues) {
        std::cout << stringPair.first << ":\n.ascii \"";
        writeString(std::cout, stringPair.second );
        std::cout<< "\\0\"\n";
    }
    std::cout << ".globl main\n";
    for (BasicBlock *basicBlock : controlFlowGraph->basicBlocks) {
        int ind = -1;
        actualBasicBlock = basicBlock;
        allocator->actualBasicBlock = basicBlock;
        allocator->initValues();
        isBlockEndedWithJump = false;
        for (auto it = basicBlock->liveness.rbegin(); it != basicBlock->liveness.rend(); it++) {
            if (ind >= 0) {
                basicBlock->quadlist[ind]->accept(this);
            }
            allocator->removeDeadValues(*it);
            ind++;
        }
        if (!isBlockEndedWithJump && !basicBlock->successors.empty()) {
            //  allocator->writeLiveValues();
            if (!actualBasicBlock->successors.empty()) {
                allocator->moveLocalVariables(*actualBasicBlock->liveness.begin(), actualBasicBlock->memoryMap,
                                              (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                                      *actualBasicBlock->successors.begin())->second]->memoryMap,
                                              isAfterCall);
            }
        }
        isAfterCall = false;
    }
}

int CodeGenerator::getNumOfLocalVariables(Ident funIdent) {
    int max = 0;
    for (auto basicBlock : controlFlowGraph->basicBlocks) {
        if (basicBlock->funIdent == funIdent && basicBlock->memoryMap.size() > max) {
            max = basicBlock->memoryMap.size();
        }
    }
    return max;
}

void CodeGenerator::printRegisterAllocation() {
    std::cout << "\n\n";
    for (auto value : registerAllocationMap) {
        std::cout << value.first << " " << value.second << "\n";
    }
    std::cout << "\n\n";

}

void CodeGenerator::pushCallPreservedRegisters() {
    std::cout << "pushq %rbx\n";
    std::cout << "pushq %r12\n";
    std::cout << "pushq %r13\n";
    std::cout << "pushq %r14\n";
    std::cout << "pushq %r15\n";
}

void CodeGenerator::popCallPreservedRegisters() {
    std::cout << "popq %r15\n";
    std::cout << "popq %r14\n";
    std::cout << "popq %r13\n";
    std::cout << "popq %r12\n";
    std::cout << "popq %rbx\n";

}

std::ostream &CodeGenerator::writeString(std::ostream &out, std::string const &s) {
    for (auto ch : s) {
        switch (ch) {
            case '\'':
                out << "\\'";
                break;

            case '\"':
                out << "\\\"";
                break;

            case '\?':
                out << "\\?";
                break;

            case '\\':
                out << "\\\\";
                break;

            case '\a':
                out << "\\a";
                break;

            case '\b':
                out << "\\b";
                break;

            case '\f':
                out << "\\f";
                break;

            case '\n':
                out << "\\n";
                break;

            case '\r':
                out << "\\r";
                break;

            case '\t':
                out << "\\t";
                break;

            case '\v':
                out << "\\v";
                break;

            default:
                out << ch;
        }
    }

    return out;
}
