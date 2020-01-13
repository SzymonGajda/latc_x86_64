//
// Created by szymon on 02.01.2020.
//

#ifndef LATC_X86_64_LIVENESSANALYSER_H
#define LATC_X86_64_LIVENESSANALYSER_H


#include <set>
#include "BaseVisitor.H"

class LivenessAnalyser : public BaseVisitor{

public:
    std::set<String> created;
    std::set<String> destroyed;

    void visitQuadAss1(QuadAss1 *q);

    void  visitQuadAss2(QuadAss2 *q);

    void  visitQuadCopy(QuadCopy *q);

    void  visitQuadJmp(QuadJmp *q);

    void  visitQuadLabel(QuadLabel *q);

    void  visitQuadIf(QuadIf *q);

    void  visitQuadParam(QuadParam *q);

    void  visitQuadCall(QuadCall *q);

    void  visitQuadReturn(QuadReturn *q);

    void visitQuadReturnNoVal(QuadReturnNoVal *q);

    void visitQuadRetrieve(QuadRetrieve *q);

    void visitQuadFunBegin(QuadFunBegin *q);

};


#endif //LATC_X86_64_LIVENESSANALYSER_H
