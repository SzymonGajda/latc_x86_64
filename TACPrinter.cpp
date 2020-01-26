//
// Created by szymon on 02.01.2020.
//

#include <iostream>
#include "TACPrinter.h"

void TACPrinter::visitQuadBlk(QuadBlk *q) {
    for (auto it = q->quadlist->begin(); it != q->quadlist->end(); it++) {
        (*it)->accept(this);
    }
}

void TACPrinter::visitQuadAss1(QuadAss1 *q) {
    if (q->arg.isValue) {
        std::cout << q->res << " := " << q->op << q->arg.value << "\n";
    } else {
        std::cout << q->res << " := " << q->op << "[" << q->arg.var << "]" << "\n";
    }
}

void TACPrinter::visitQuadAss2(QuadAss2 *q) {
    std::cout << q->res << " := ";
    if (q->arg1.isValue) {
        std::cout << q->arg1.value;
    } else {
        std::cout << "[" << q->arg1.var << "]";
    }
    std::cout << " " << q->op << " ";
    if (q->arg2.isValue) {
        std::cout << q->arg2.value;
    } else {
        std::cout << "[" << q->arg2.var << "]";
    }
    std::cout << "\n";
}

void TACPrinter::visitQuadCopy(QuadCopy *q) {
    std::cout << q->res << " := ";
    if (q->arg.isValue) {
        std::cout << q->arg.value;
    } else {
        std::cout << "[" << q->arg.var << "]";
    }
    std::cout << "\n";
}

void TACPrinter::visitQuadJmp(QuadJmp *q) {
    std::cout << "goto " << q->label << "\n";
}

void TACPrinter::visitQuadLabel(QuadLabel *q) {
    std::cout << q->label << ":\n";
}

void TACPrinter::visitQuadIf(QuadIf *q) {
    std::cout << "if ";
    if (q->cond.isValue) {
        std::cout << q->cond.value;
    } else {
        std::cout << "[" << q->cond.var << "]";
    }
    std::cout << " then " << q->label1 << " else " << q->label2 << "\n";
}

void TACPrinter::visitQuadParam(QuadParam *q) {
    std::cout << "param ";
    if (q->arg.isValue) {
        std::cout << q->arg.value;
    } else {
        std::cout << "[" << q->arg.var << "]";
    }
    std::cout << "\n";
}

void TACPrinter::visitQuadCall(QuadCall *q) {
    std::cout << "call " << q->label << " " << q->argsNum << "\n";
}

void TACPrinter::visitQuadReturn(QuadReturn *q) {
    std::cout << "return ";
    if (q->ret.isValue) {
        std::cout << q->ret.value;
    } else {
        std::cout << "[" << q->ret.var << "]";
    }
    std::cout << "\n";
}

void TACPrinter::visitQuadReturnNoVal(QuadReturnNoVal *q) {
    std::cout << "return\n";
}

void TACPrinter::visitQuadRetrieve(QuadRetrieve *q) {
    std::cout << "retrieve " << q->res << "\n";
}

void TACPrinter::visitQuadFunBegin(QuadFunBegin *q) {
    std::cout << "fun definition: " << q->ident << "\n";
}


