//
// Created by szymon on 06.01.2020.
//

#include <iostream>
#include "CodeGenerator.h"
#include "GlobalAllocator.h"

void CodeGenerator::visitQuadAss1(QuadAss1 *q) {
    if (q->arg.isValue) {
        allocator->genAss1(q->res, q->arg.value, q->op);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    } else {
        allocator->genAss1(q->res, q->arg.var, q->op);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
}

void CodeGenerator::visitQuadAss2(QuadAss2 *q) {
    if (q->arg1.isValue && q->arg2.isValue) {
        allocator->genAss2(q->res, q->arg1.value, q->arg2.value, q->op);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
    if (!q->arg1.isValue && q->arg2.isValue) {
        allocator->genAss2(q->res, q->arg1.var, q->arg2.value, q->op);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
    if (q->arg1.isValue && !q->arg2.isValue) {
        allocator->genAss2(q->res, q->arg1.value, q->arg2.var, q->op);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
    if (!q->arg1.isValue && !q->arg2.isValue) {
        if (q->arg1.type == "string") {
            allocator->addString(q->res, q->arg1.var, q->arg2.var);
            resultCode += allocator->resultCode;
            allocator->resultCode = "";
        } else {
            allocator->genAss2(q->res, q->arg1.var, q->arg2.var, q->op);
            resultCode += allocator->resultCode;
            allocator->resultCode = "";
        }
    }
}

void CodeGenerator::visitQuadCopy(QuadCopy *q) {
    if (q->arg.isValue) {
        if (q->arg.type == "string") {
            allocator->copyString(q->res, q->arg.stringValue);
            resultCode += allocator->resultCode;
            allocator->resultCode = "";
        } else {
            allocator->copy(q->res, q->arg.value);
            resultCode += allocator->resultCode;
            allocator->resultCode = "";
        }
    } else {
        allocator->copy(q->res, q->arg.var);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
}

void CodeGenerator::visitQuadJmp(QuadJmp *q) {
    isBlockEndedWithJump = true;
    allocator->moveLocalVariables(*actualBasicBlock->liveness.begin(), actualBasicBlock->memoryMap,
                                  (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                          q->label)->second]->memoryMap, false);
    resultCode += allocator->resultCode;
    allocator->resultCode = "";
    resultCode += q->jmpOp + " " + q->label + "\n";
}

void CodeGenerator::visitQuadLabel(QuadLabel *q) {
    resultCode += q->label + ":\n";
}

void CodeGenerator::visitQuadIf(QuadIf *q) {
    isBlockEndedWithJump = true;
    if (q->cond.isValue) {
        if (q->cond.value == 0) {
            allocator->moveLocalVariables(*actualBasicBlock->liveness.begin(), actualBasicBlock->memoryMap,
                                          (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                                  q->label2)->second]->memoryMap, false);
            resultCode += allocator->resultCode;
            allocator->resultCode = "";
            resultCode += "jmp " + q->label2 + "\n";
        } else {
            allocator->moveLocalVariables(*actualBasicBlock->liveness.begin(), actualBasicBlock->memoryMap,
                                          (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                                  q->label1)->second]->memoryMap, false);
            resultCode += allocator->resultCode;
            allocator->resultCode = "";
            resultCode += "jmp " + q->label1 + "\n";
        }
    } else {
        allocator->genIf(q->cond.var, q->label1, q->label2, *actualBasicBlock->liveness.begin(),
                         actualBasicBlock->memoryMap,
                         (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                 q->label1)->second]->memoryMap,
                         (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                 q->label2)->second]->memoryMap);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
}

void CodeGenerator::visitQuadParam(QuadParam *q) {
    if (paramNum == -1 && !actualBasicBlock->successors.empty()) {
        allocator->writeLiveValues();
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
    if (paramNum == -1) {
        int argNum = functionHeaders->getHeader(q->ident).args.size();
        if (argNum > 6) {
            if ((stackAlignment->stackAlignment + ((argNum - 6) * 8) + 8) % 16 != 0) {
                resultCode += "subq $8, %rsp\n";
                stackAlignment->isStackAlignedBeforeCall = true;
            }
        } else {
            if ((stackAlignment->stackAlignment + 8) % 16 != 0) {
                resultCode += "subq $8, %rsp\n";
                stackAlignment->isStackAlignedBeforeCall = true;
            }
        }
    }
    if (q->arg.isValue) {
        allocator->genParam(paramNum, q->arg.value);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    } else {
        allocator->genParam(paramNum, q->arg.var);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
    paramNum++;
}

void CodeGenerator::visitQuadCall(QuadCall *q) {
    if (paramNum == -1 && !actualBasicBlock->successors.empty()) {
        allocator->writeLiveValues();
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
    if (paramNum == -1 && !stackAlignment->isStackAlignedBeforeCall) {
        if ((stackAlignment->stackAlignment + 8) % 16 != 0) {
            resultCode += "subq $8, %rsp\n";
            stackAlignment->isStackAlignedBeforeCall = true;
        }
    }
    paramNum = -1;
    resultCode += "pushq %r11\n";
    resultCode += "call " + q->label + "\n";
    resultCode += "popq %r11\n";
    allocator->popFunArgs(q->argsNum);
    if (stackAlignment->isStackAlignedBeforeCall) {
        resultCode += "add $8, %rsp\n";
        stackAlignment->isStackAlignedBeforeCall = false;
    }
    if (functionHeaders->getHeader(q->label).returnType == "void") {
        allocator->clearRegistersInfo();
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
    isAfterCall = true;
}

void CodeGenerator::visitQuadReturn(QuadReturn *q) {
    if (q->ret.isValue) {
        allocator->genRet(q->ret.value);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    } else {
        allocator->genRet(q->ret.var);
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
    }
}

void CodeGenerator::visitQuadReturnNoVal(QuadReturnNoVal *q) {
    if (actualBasicBlock->funIdent != "main") {
        popCallPreservedRegisters();
    }
    resultCode += "movq %rbp, %rsp\n";
    resultCode += "popq %rbp\n";
    resultCode += "ret\n";
}

void CodeGenerator::visitQuadRetrieve(QuadRetrieve *q) {
    allocator->genRetrieve(q->res);
    allocator->saveRax();
    allocator->clearRegistersInfo();
    resultCode += allocator->resultCode;
    allocator->resultCode = "";
}

void CodeGenerator::visitQuadFunBegin(QuadFunBegin *q) {
    GlobalAllocator globalAllocator;
    stackAlignment->stackAlignment = 8;
    stackAlignment->isStackAlignedBeforeCall = false;
    registerAllocationMap = globalAllocator.allocateRegisters(q->ident, controlFlowGraph);
    allocator->registerAllocationMap = registerAllocationMap;
    int numOfVariables = getNumOfLocalVariables(q->ident);
    resultCode += q->ident + ":" + "\n";
    resultCode += "pushq %rbp\n";
    resultCode += "movq %rsp, %rbp\n";
    resultCode += "subq $" + std::to_string(numOfVariables * 8) + ", %rsp\n";
    stackAlignment->stackAlignment += (numOfVariables * 8) + 8;
    if (q->ident != "main") {
        pushCallPreservedRegisters();
    }
    allocator->initFunArgs(functionHeaders->getHeader(q->ident).newArgSymbols, *actualBasicBlock->liveness.rbegin(),
                           actualBasicBlock->memoryMap);
    resultCode += allocator->resultCode;
    allocator->resultCode = "";
}

void CodeGenerator::generateCode() {
    allocator->initRegisters();
    allocator->initRegistersIdentMap();
    allocator->stackAlignment = stackAlignment;
    resultCode += allocator->resultCode;
    allocator->resultCode = "";
    resultCode += ".extern printInt\n"
                  ".extern printString\n"
                  ".extern readInt\n"
                  ".extern readString\n"
                  ".extern error\n"
                  ".extern _concat\n"
                  ".text\n";
    for (auto stringPair: stringValues) {
        resultCode += stringPair.first + ":\n.ascii \"";
        resultCode += writeString(stringPair.second);
        resultCode += "\\0\"\n";
    }
    resultCode += ".globl main\n";
    for (BasicBlock *basicBlock : controlFlowGraph->basicBlocks) {
        int ind = -1;
        actualBasicBlock = basicBlock;
        allocator->actualBasicBlock = basicBlock;
        allocator->initValues();
        resultCode += allocator->resultCode;
        allocator->resultCode = "";
        isBlockEndedWithJump = false;
        for (auto it = basicBlock->liveness.rbegin(); it != basicBlock->liveness.rend(); it++) {
            if (ind >= 0) {
                basicBlock->quadlist[ind]->accept(this);
            }
            allocator->removeDeadValues(*it);
            ind++;
        }
        if (!isBlockEndedWithJump && !basicBlock->successors.empty()) {
            if (!actualBasicBlock->successors.empty()) {
                allocator->moveLocalVariables(*actualBasicBlock->liveness.begin(), actualBasicBlock->memoryMap,
                                              (controlFlowGraph->basicBlocks)[controlFlowGraph->blockLabelsMap.find(
                                                      *actualBasicBlock->successors.begin())->second]->memoryMap,
                                              isAfterCall);
                resultCode += allocator->resultCode;
                allocator->resultCode = "";
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
    resultCode += "\n\n";
    for (auto value : registerAllocationMap) {
        std::cout << value.first << " " << value.second << "\n";
    }
    resultCode += "\n\n";

}

void CodeGenerator::pushCallPreservedRegisters() {
    resultCode += "pushq %rbx\n";
    resultCode += "pushq %r12\n";
    resultCode += "pushq %r13\n";
    resultCode += "pushq %r14\n";
    resultCode += "pushq %r15\n";
    stackAlignment->stackAlignment += 5 * 8;
}

void CodeGenerator::popCallPreservedRegisters() {
    resultCode += "popq %r15\n";
    resultCode += "popq %r14\n";
    resultCode += "popq %r13\n";
    resultCode += "popq %r12\n";
    resultCode += "popq %rbx\n";
}

std::string CodeGenerator::writeString(std::string const &s) {
    String out = "";
    for (auto ch : s) {
        switch (ch) {
            case '\'':
                out += "\\'";
                break;

            case '\"':
                out += "\\\"";
                break;

            case '\?':
                out += "\\?";
                break;

            case '\\':
                out += "\\\\";
                break;

            case '\a':
                out += "\\a";
                break;

            case '\b':
                out += "\\b";
                break;

            case '\f':
                out += "\\f";
                break;

            case '\n':
                out += "\\n";
                break;

            case '\r':
                out += "\\r";
                break;

            case '\t':
                out += "\\t";
                break;

            case '\v':
                out += "\\v";
                break;

            default:
                out += ch;
        }
    }

    return out;
}
