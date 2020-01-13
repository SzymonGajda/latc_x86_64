//
// Created by szymon on 02.01.2020.
//

#include "LivenessAnalyser.h"

void LivenessAnalyser::visitQuadAss1(QuadAss1 *q) {
    destroyed.insert(q->res);
    if(!q->arg.isValue){
        created.insert(q->arg.var);
    }
}

void LivenessAnalyser::visitQuadAss2(QuadAss2 *q) {
    destroyed.insert(q->res);
    if(!q->arg1.isValue){
        created.insert(q->arg1.var);
    }
    if(!q->arg2.isValue){
        created.insert(q->arg2.var);
    }
}

void LivenessAnalyser::visitQuadCopy(QuadCopy *q) {
    destroyed.insert(q->res);
    if(!q->arg.isValue){
        created.insert(q->arg.var);
    }
}

void LivenessAnalyser::visitQuadJmp(QuadJmp *q) {

}

void LivenessAnalyser::visitQuadLabel(QuadLabel *q) {

}

void LivenessAnalyser::visitQuadIf(QuadIf *q) {
    if(!q->cond.isValue){
        created.insert(q->cond.var);
    }
}

void LivenessAnalyser::visitQuadParam(QuadParam *q) {
    if(!q->arg.isValue){
        created.insert(q->arg.var);
    }
}

void LivenessAnalyser::visitQuadCall(QuadCall *q) {

}

void LivenessAnalyser::visitQuadReturn(QuadReturn *q) {
    if(!q->ret.isValue){
        created.insert(q->ret.var);
    }
}

void LivenessAnalyser::visitQuadReturnNoVal(QuadReturnNoVal *q) {

}

void LivenessAnalyser::visitQuadRetrieve(QuadRetrieve *q) {
    created.insert(q->res);
}

void LivenessAnalyser::visitQuadFunBegin(QuadFunBegin *q) {

}
