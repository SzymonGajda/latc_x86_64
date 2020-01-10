//
// Created by szymon on 06.01.2020.
//

#ifndef LATC_X86_64_CODEGENERATOR_H
#define LATC_X86_64_CODEGENERATOR_H


#include "BaseVisitor.H"
#include "ControlFlowGraph.h"
#include "Allocator.h"

class CodeGenerator : public BaseVisitor {
public:
    ControlFlowGraph *controlFlowGraph;
    Allocator *allocator;

    void generateCode();

    void visitQuadAss1(QuadAss1 *q);

    void  visitQuadAss2(QuadAss2 *q);

    void  visitQuadCopy(QuadCopy *q);

    void  visitQuadGoto(QuadGoto *q);

    void  visitQuadLabel(QuadLabel *q);

    void  visitQuadIf(QuadIf *q);

    void  visitQuadParam(QuadParam *q);

    void  visitQuadCall(QuadCall *q);

    void  visitQuadReturn(QuadReturn *q);

    void visitQuadReturnNoVal(QuadReturnNoVal *q);

    void visitQuadRetrieve(QuadRetrieve *q);

    void visitQuadFunBegin(QuadFunBegin *q);
};


#endif //LATC_X86_64_CODEGENERATOR_H
