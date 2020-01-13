//
// Created by szymon on 06.01.2020.
//

#include <iostream>
#include "CodeGenerator.h"

void CodeGenerator::visitQuadAss1(QuadAss1 *q) {
    if(q->arg.isValue){
        allocator->genAss1(q->res, q->arg.value, q->op);
    } else{
        allocator->genAss1(q->res, q->arg.var, q->op);
    }
}

void CodeGenerator::visitQuadAss2(QuadAss2 *q) {
    if(q->arg1.isValue && q->arg2.isValue){
        allocator->genAss2(q->res, q->arg1.value, q->arg2.value, q->op);
    }
    if(!q->arg1.isValue && q->arg2.isValue){
        allocator->genAss2(q->res, q->arg1.var, q->arg2.value, q->op);
    }
    if(q->arg1.isValue && !q->arg2.isValue){
        allocator->genAss2(q->res, q->arg1.value, q->arg2.var, q->op);
    }
    if(!q->arg1.isValue && !q->arg2.isValue){
        allocator->genAss2(q->res, q->arg1.var, q->arg2.var, q->op);
    }
}

void CodeGenerator::visitQuadCopy(QuadCopy *q) {
    if(q->arg.isValue){
        allocator->copy(q->res, q->arg.value);
    }
    else{
        allocator->copy(q->res, q->arg.var);
    }
}

void CodeGenerator::visitQuadJmp(QuadJmp *q) {
    std::cout<<q->jmpOp<<" "<<q->label<<"\n";
}

void CodeGenerator::visitQuadLabel(QuadLabel *q) {
    std::cout<<q->label<<":\n";
}

void CodeGenerator::visitQuadIf(QuadIf *q) {
    if(q->cond.isValue){
        if(q->cond.value == 0){
            std::cout<<"jmp "<<q->label2<<"\n";
        } else{
            std::cout<<"jmp "<<q->label1<<"\n";
        }
    }
    else{
        allocator->genIf(q->cond.var, q->label1, q->label2);
    }
}

void CodeGenerator::visitQuadParam(QuadParam *q) {
    BaseVisitor::visitQuadParam(q);
}

void CodeGenerator::visitQuadCall(QuadCall *q) {
    BaseVisitor::visitQuadCall(q);
}

void CodeGenerator::visitQuadReturn(QuadReturn *q) {
    if(q->ret.isValue){
        allocator->genRet(q->ret.value);
    }else{
        allocator->genRet(q->ret.var);
    }
}

void CodeGenerator::visitQuadReturnNoVal(QuadReturnNoVal *q) {
    std::cout<<"ret\n";
}

void CodeGenerator::visitQuadRetrieve(QuadRetrieve *q) {
    //BaseVisitor::visitQuadRetrieve(q);

}

void CodeGenerator::visitQuadFunBegin(QuadFunBegin *q) {
    std::cout<<q->ident<<":"<<"\n";
}

void CodeGenerator::generateCode() {
    allocator->initRegisters();
    allocator->initRegistersIdentMap();
    std::cout<<".extern printInt\n"
               ".extern printString\n"
               ".extern readInt\n"
               ".extern readString\n"
               ".text\n"
               ".globl main\n";
    for(BasicBlock *basicBlock : controlFlowGraph->basicBlocks){
        int ind = -1;
        for(auto it = basicBlock->liveness.rbegin(); it!=basicBlock->liveness.rend();it++ ){
            if(ind >= 0){
                basicBlock->quadlist[ind]->accept(this);
            }
            allocator->removeDeadValues(*it);
            ind++;
        }
    }
}
