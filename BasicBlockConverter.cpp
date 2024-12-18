//
// Created by szymon on 02.01.2020.
//

#include <iostream>
#include "BasicBlockConverter.h"
#include "TACPrinter.h"
#include "CodeGenerator.h"


void BasicBlockConverter::visitProg(Prog *q) {
    q->listtopdef_->accept(this);
    setEdges();
    controlFlowGraph->calculateDataFlow();
    controlFlowGraph->generateMemoryMap(symbolsTable);
}

void BasicBlockConverter::visitQuadBlk(QuadBlk *q) {
    if (!basicBlock->quadlist.empty()) {
        basicBlock = new BasicBlock;
    }
    for (auto it = q->quadlist->begin(); it != q->quadlist->end(); it++) {
        (*it)->accept(this);
    }
    if (!basicBlock->quadlist.empty()) {
        controlFlowGraph->basicBlocks.push_back(basicBlock);
    }
    delete q;
}

void BasicBlockConverter::visitQuadFunBegin(QuadFunBegin *q) {
    actualFun = q->ident;
    if (!basicBlock->quadlist.empty()) {
        basicBlock->num = controlFlowGraph->basicBlocks.size();
        setMap();
        controlFlowGraph->basicBlocks.push_back(basicBlock);
        basicBlock = new BasicBlock;
        basicBlock->funIdent = actualFun;
        basicBlock->ident = q->ident;
    } else {
        basicBlock->ident = q->ident;
        basicBlock->funIdent = actualFun;
    }
    basicBlock->quadlist.push_back(q);
}


void BasicBlockConverter::visitQuadAss1(QuadAss1 *q) {
    basicBlock->quadlist.push_back(q);
}

void BasicBlockConverter::visitQuadAss2(QuadAss2 *q) {
    basicBlock->quadlist.push_back(q);
}

void BasicBlockConverter::visitQuadCopy(QuadCopy *q) {
    basicBlock->quadlist.push_back(q);
}

void BasicBlockConverter::visitQuadJmp(QuadJmp *q) {
    basicBlock->quadlist.push_back(q);
    basicBlock->successors.insert(q->label);
    basicBlock->num = controlFlowGraph->basicBlocks.size();
    basicBlock->successors.insert(q->label);
    setMap();
    controlFlowGraph->basicBlocks.push_back(basicBlock);
    basicBlock = new BasicBlock;
    basicBlock->funIdent = actualFun;
}

void BasicBlockConverter::visitQuadLabel(QuadLabel *q) {
    if (!basicBlock->quadlist.empty()) {
        basicBlock->num = controlFlowGraph->basicBlocks.size();
        basicBlock->successors.insert(q->label);
        setMap();
        controlFlowGraph->basicBlocks.push_back(basicBlock);
        basicBlock = new BasicBlock;
        basicBlock->funIdent = actualFun;
        basicBlock->ident = q->label;
    } else {
        basicBlock->ident = q->label;
        basicBlock->funIdent = actualFun;
    }
    basicBlock->quadlist.push_back(q);
}

void BasicBlockConverter::visitQuadIf(QuadIf *q) {
    basicBlock->quadlist.push_back(q);
    basicBlock->successors.insert(q->label1);
    basicBlock->successors.insert(q->label2);
}

void BasicBlockConverter::visitQuadParam(QuadParam *q) {
    basicBlock->quadlist.push_back(q);
}

void BasicBlockConverter::visitQuadCall(QuadCall *q) {
    basicBlock->quadlist.push_back(q);
}

void BasicBlockConverter::visitQuadReturn(QuadReturn *q) {
    basicBlock->quadlist.push_back(q);
    basicBlock->num = controlFlowGraph->basicBlocks.size();
    setMap();
    controlFlowGraph->basicBlocks.push_back(basicBlock);
    basicBlock = new BasicBlock;
    basicBlock->funIdent = actualFun;
}

void BasicBlockConverter::visitQuadReturnNoVal(QuadReturnNoVal *q) {
    basicBlock->quadlist.push_back(q);
    basicBlock->num = controlFlowGraph->basicBlocks.size();
    setMap();
    controlFlowGraph->basicBlocks.push_back(basicBlock);
    basicBlock = new BasicBlock;
    basicBlock->funIdent = actualFun;
}

void BasicBlockConverter::visitQuadRetrieve(QuadRetrieve *q) {
    basicBlock->quadlist.push_back(q);
}

void BasicBlockConverter::printBlock() {
    std::cout << "\n\nNEW BLOCK " << basicBlock->num << " " << basicBlock->ident << ":\n";
    for (Quadruple *quadruple : basicBlock->quadlist) {
        quadruple->accept(tacPrinter);
    }
}

void BasicBlockConverter::setMap() {
    if (basicBlock->ident != "") {
        controlFlowGraph->blockLabelsMap[basicBlock->ident] = basicBlock->num;
    }
}

void BasicBlockConverter::setEdges() {
    for (BasicBlock *basicBlock1 : controlFlowGraph->basicBlocks) {
        for (String ident : basicBlock1->successors) {
            int predNum = controlFlowGraph->blockLabelsMap[ident];
            controlFlowGraph->basicBlocks[predNum]->predecessors.insert(basicBlock1->num);
        }
    }
}





