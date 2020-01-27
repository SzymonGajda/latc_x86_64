//
// Created by szymon on 06.01.2020.
//

#ifndef LATC_X86_64_CODEGENERATOR_H
#define LATC_X86_64_CODEGENERATOR_H


#include "BaseVisitor.H"
#include "ControlFlowGraph.h"
#include "Allocator.h"
#include "FunctionHeader.h"



class CodeGenerator : public BaseVisitor {
public:
    ControlFlowGraph *controlFlowGraph;
    Allocator *allocator;
    BasicBlock *actualBasicBlock;
    StackAlignment *stackAlignment;
    bool isBlockEndedWithJump;
    Ident actualFun;
    FunctionHeaders *functionHeaders;
    std::vector<std::pair<String, String> > stringValues;
    std::map<Ident, int> registerAllocationMap;
    int paramNum = -1;
    bool isAfterCall = false;

    String resultCode = "";

    int getNumOfLocalVariables(Ident funIdent);

    void printRegisterAllocation();

    void generateCode();

    void pushCallPreservedRegisters();

    void popCallPreservedRegisters();

    std::string writeString(std::string const &s);

    void visitQuadAss1(QuadAss1 *q);

    void visitQuadAss2(QuadAss2 *q);

    void visitQuadCopy(QuadCopy *q);

    void visitQuadJmp(QuadJmp *q);

    void visitQuadLabel(QuadLabel *q);

    void visitQuadIf(QuadIf *q);

    void visitQuadParam(QuadParam *q);

    void visitQuadCall(QuadCall *q);

    void visitQuadReturn(QuadReturn *q);

    void visitQuadReturnNoVal(QuadReturnNoVal *q);

    void visitQuadRetrieve(QuadRetrieve *q);

    void visitQuadFunBegin(QuadFunBegin *q);
};


#endif //LATC_X86_64_CODEGENERATOR_H
