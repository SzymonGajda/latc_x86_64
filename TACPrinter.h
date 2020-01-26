//
// Created by szymon on 02.01.2020.
//

#ifndef LATC_X86_64_TACPRINTER_H
#define LATC_X86_64_TACPRINTER_H


#include "BaseVisitor.H"

class TACPrinter : public BaseVisitor {

public:

    void visitQuadBlk(QuadBlk *q);

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


#endif //LATC_X86_64_TACPRINTER_H
