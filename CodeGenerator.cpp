//
// Created by szymon on 06.01.2020.
//

#include "CodeGenerator.h"

void CodeGenerator::visitQuadAss1(QuadAss1 *q) {
    
}

void CodeGenerator::visitQuadAss2(QuadAss2 *q) {
    BaseVisitor::visitQuadAss2(q);
}

void CodeGenerator::visitQuadCopy(QuadCopy *q) {
    if(q->arg.isValue){
        allocator->copy(q->res, q->arg.value);
    }
    else{
        allocator->copy(q->res, q->arg.var);
    }
}

void CodeGenerator::visitQuadGoto(QuadGoto *q) {
    BaseVisitor::visitQuadGoto(q);
}

void CodeGenerator::visitQuadLabel(QuadLabel *q) {
    BaseVisitor::visitQuadLabel(q);
}

void CodeGenerator::visitQuadIf(QuadIf *q) {
    BaseVisitor::visitQuadIf(q);
}

void CodeGenerator::visitQuadParam(QuadParam *q) {
    BaseVisitor::visitQuadParam(q);
}

void CodeGenerator::visitQuadCall(QuadCall *q) {
    BaseVisitor::visitQuadCall(q);
}

void CodeGenerator::visitQuadReturn(QuadReturn *q) {
    BaseVisitor::visitQuadReturn(q);
}

void CodeGenerator::visitQuadReturnNoVal(QuadReturnNoVal *q) {
    BaseVisitor::visitQuadReturnNoVal(q);
}

void CodeGenerator::visitQuadRetrieve(QuadRetrieve *q) {
    BaseVisitor::visitQuadRetrieve(q);
}

void CodeGenerator::visitQuadFunBegin(QuadFunBegin *q) {
    BaseVisitor::visitQuadFunBegin(q);
}
