//
// Created by szymon on 30.12.2019.
//

#include "ThreeAddressCodeConverter.h"

void ThreeAddressCodeConverter::visitFnDef(FnDef *p) {
    FunctionHeader functionHeader = functionHeaders->getHeader(p->ident_);
    symbolsTable->entryScope();
    std::vector<Ident> newFunctionArgIdents;
    for (std::pair<String, String> pair : functionHeader.args) {
        String newSymbol = symbolsTable->getNewSymbol();
        newFunctionArgIdents.push_back(newSymbol);
        symbolsTable->addSymbol(pair.second, newSymbol, pair.first);
    }
    functionHeader.newArgSymbols = newFunctionArgIdents;
    functionHeaders->removeHeader(p->ident_);
    functionHeaders->add(functionHeader);
    quadBlk = new QuadBlk();
    QuadFunBegin *quadFunBegin = new QuadFunBegin(p->ident_);
    quadBlk->quadlist->push_back(quadFunBegin);
    p->block_->accept(this);
    symbolsTable->exitScope();
    delete p->block_;
    if (functionHeader.returnType == "void") {
        QuadReturnNoVal *quadReturnNoVal = new QuadReturnNoVal;
        quadBlk->quadlist->push_back(quadReturnNoVal);
    }
    p->block_ = quadBlk;
}

void ThreeAddressCodeConverter::visitBlk(Blk *p) {
    symbolsTable->entryScope();
    p->liststmt_->accept(this);
    symbolsTable->exitScope();
}

void ThreeAddressCodeConverter::visitDecl(Decl *p) {
    p->type_->accept(this);
    declType = type;
    p->listitem_->accept(this);
}

void ThreeAddressCodeConverter::visitAss(Ass *p) {
    p->expr_->accept(this);
    Ident ident = symbolsTable->getSymbol(p->ident_);
    QuadCopy *copy = new QuadCopy(ident, arg);
    quadBlk->quadlist->push_back(copy);
}

void ThreeAddressCodeConverter::visitIncr(Incr *p) {
    Ident ident = symbolsTable->getSymbol(p->ident_);
    QuadArg arg1 = QuadArg(false, 0, ident);
    QuadArg arg2 = QuadArg(true, 1, "");
    QuadAss2 *ass = new QuadAss2(ident, arg1, arg2, "add");
    quadBlk->quadlist->push_back(ass);
}

void ThreeAddressCodeConverter::visitDecr(Decr *p) {
    Ident ident = symbolsTable->getSymbol(p->ident_);
    QuadArg arg1 = QuadArg(false, 0, ident);
    QuadArg arg2 = QuadArg(true, 1, "");
    QuadAss2 *ass = new QuadAss2(ident, arg1, arg2, "sub");
    quadBlk->quadlist->push_back(ass);
}

void ThreeAddressCodeConverter::visitRet(Ret *p) {
    p->expr_->accept(this);
    QuadArg val = arg;
    QuadReturn *quadReturn = new QuadReturn(val);
    quadBlk->quadlist->push_back(quadReturn);
}

void ThreeAddressCodeConverter::visitVRet(VRet *p) {
    QuadReturnNoVal *quadReturnNoVal = new QuadReturnNoVal();
    quadBlk->quadlist->push_back(quadReturnNoVal);
}

void ThreeAddressCodeConverter::visitCond(Cond *p) {
    Ident trueLabel = symbolsTable->getNewSymbol();
    Ident falseLabel = symbolsTable->getNewSymbol();
    p->expr_->accept(this);
    QuadIf *quadIf = new QuadIf(arg, trueLabel, falseLabel);
    quadBlk->quadlist->push_back(quadIf);
    QuadLabel *quadLabelTrue = new QuadLabel(trueLabel);
    quadBlk->quadlist->push_back(quadLabelTrue);
    p->stmt_->accept(this);
    QuadLabel *quadLabelFalse = new QuadLabel(falseLabel);
    quadBlk->quadlist->push_back(quadLabelFalse);
}

void ThreeAddressCodeConverter::visitCondElse(CondElse *p) {
    Ident trueLabel = symbolsTable->getNewSymbol();
    Ident falseLabel = symbolsTable->getNewSymbol();
    Ident endLabel = symbolsTable->getNewSymbol();
    p->expr_->accept(this);
    QuadIf *quadIf = new QuadIf(arg, trueLabel, falseLabel);
    quadBlk->quadlist->push_back(quadIf);
    QuadLabel *quadLabelTrue = new QuadLabel(trueLabel);
    quadBlk->quadlist->push_back(quadLabelTrue);
    p->stmt_1->accept(this);
    QuadJmp *quadGoto = new QuadJmp(endLabel, "jmp");
    quadBlk->quadlist->push_back(quadGoto);
    QuadLabel *quadLabelFalse = new QuadLabel(falseLabel);
    quadBlk->quadlist->push_back(quadLabelFalse);
    p->stmt_2->accept(this);
    QuadLabel *quadLabelEnd = new QuadLabel(endLabel);
    quadBlk->quadlist->push_back(quadLabelEnd);
}

void ThreeAddressCodeConverter::visitWhile(While *p) {
    Ident startLabel = symbolsTable->getNewSymbol();
    Ident trueLabel = symbolsTable->getNewSymbol();
    Ident falseLabel = symbolsTable->getNewSymbol();
    QuadLabel *quadLabelStart = new QuadLabel(startLabel);
    quadBlk->quadlist->push_back(quadLabelStart);
    p->expr_->accept(this);
    QuadIf *quadIf = new QuadIf(arg, trueLabel, falseLabel);
    quadBlk->quadlist->push_back(quadIf);
    QuadLabel *quadLabelTrue = new QuadLabel(trueLabel);
    quadBlk->quadlist->push_back(quadLabelTrue);
    p->stmt_->accept(this);
    QuadJmp *quadGoto = new QuadJmp(startLabel, "jmp");
    quadBlk->quadlist->push_back(quadGoto);
    QuadLabel *quadLabelFalse = new QuadLabel(falseLabel);
    quadBlk->quadlist->push_back(quadLabelFalse);
}

void ThreeAddressCodeConverter::visitSExp(SExp *p) {
    p->expr_->accept(this);
}

void ThreeAddressCodeConverter::visitInt(Int *p) {
    type = "int";
}

void ThreeAddressCodeConverter::visitStr(Str *p) {
    type = "string";
}

void ThreeAddressCodeConverter::visitBool(Bool *p) {
    type = "boolean";
}

void ThreeAddressCodeConverter::visitVoid(Void *p) {
    type = "void";
}

void ThreeAddressCodeConverter::visitFun(Fun *p) {
    p->type_->accept(this);
}

void ThreeAddressCodeConverter::visitEVar(EVar *p) {
    QuadArg q;
    if (symbolsTable->getType(p->ident_) == "string") {
        q = QuadArg(false, "", symbolsTable->getSymbol(p->ident_));
    } else {
        q = QuadArg(false, 0, symbolsTable->getSymbol(p->ident_));
    }
    arg = q;
}

void ThreeAddressCodeConverter::visitELitInt(ELitInt *p) {
    QuadArg q = QuadArg(true, p->integer_, "");
    arg = q;
}

void ThreeAddressCodeConverter::visitELitTrue(ELitTrue *p) {
    QuadArg q = QuadArg(true, 1, "");
    arg = q;
}

void ThreeAddressCodeConverter::visitELitFalse(ELitFalse *p) {
    QuadArg q = QuadArg(true, 0, "");
    arg = q;
}

void ThreeAddressCodeConverter::visitEApp(EApp *p) {
    funIdent = p->ident_;
    p->listexpr_->accept(this);
    QuadCall *call = new QuadCall(p->ident_, p->listexpr_->size());
    quadBlk->quadlist->push_back(call);
    Ident labelIdent = symbolsTable->getNewSymbol();
    QuadLabel *quadLabel = new QuadLabel(labelIdent);
    quadBlk->quadlist->push_back(quadLabel);
    FunctionHeader functionHeader = functionHeaders->getHeader(p->ident_);
    if (functionHeader.returnType != "void") {
        Ident ident = symbolsTable->getNewSymbol();
        QuadRetrieve *quadRetrieve = new QuadRetrieve(ident);
        quadBlk->quadlist->push_back(quadRetrieve);
        arg = QuadArg(false, 0, ident);
    }
}

void ThreeAddressCodeConverter::visitEString(EString *p) {
    Ident ident1 = symbolsTable->getNewSymbol();
    Ident ident2 = symbolsTable->getNewSymbol();
    stringValues.emplace_back(ident1, p->string_);
    QuadArg q = QuadArg(true, ident1, "");
    QuadCopy *quadCopy = new QuadCopy(ident2, q);
    quadBlk->quadlist->push_back(quadCopy);
    QuadArg q2 = QuadArg(false, "", ident2);
    arg = q2;
}

void ThreeAddressCodeConverter::visitNeg(Neg *p) {
    p->expr_->accept(this);
    if (arg.isValue) {
        arg = QuadArg(true, -arg.value, "");
    } else {
        Ident res = symbolsTable->getNewSymbol();
        QuadAss1 *quadAss1 = new QuadAss1(res, arg, "neg");
        quadBlk->quadlist->push_back(quadAss1);
        arg = QuadArg(false, 0, res);
    }
}

void ThreeAddressCodeConverter::visitNot(Not *p) {
    p->expr_->accept(this);
    if (arg.isValue) {
        arg = QuadArg(true, arg.value ^ 1, "");
    } else {
        Ident res = symbolsTable->getNewSymbol();
        QuadAss2 *quadAss2 = new QuadAss2(res, arg, QuadArg(true, 1, ""), "xor");
        quadBlk->quadlist->push_back(quadAss2);
        arg = QuadArg(false, 0, res);
    }
}

void ThreeAddressCodeConverter::visitEMul(EMul *p) {
    p->mulop_->accept(this);
    String mulOp = op;
    Ident ident = symbolsTable->getNewSymbol();
    p->expr_1->accept(this);
    QuadArg quadArg1 = arg;
    p->expr_2->accept(this);
    QuadArg quadArg2 = arg;
    if (quadArg1.isValue && quadArg2.isValue) {
        if (mulOp == "imul") {
            arg = QuadArg(true, quadArg1.value * quadArg2.value, "");
        }
        if (mulOp == "idiv") {
            arg = QuadArg(true, quadArg1.value / quadArg2.value, "");
        }
        if (mulOp == "mod") {
            arg = QuadArg(true, quadArg1.value % quadArg2.value, "");
        }
    } else {
        QuadAss2 *quadAss2 = new QuadAss2(ident, quadArg1, quadArg2, mulOp);
        quadBlk->quadlist->push_back(quadAss2);
        arg = QuadArg(false, 0, ident);
    }
}

void ThreeAddressCodeConverter::visitEAdd(EAdd *p) {
    p->addop_->accept(this);
    String addOp = op;
    Ident ident = symbolsTable->getNewSymbol();
    p->expr_1->accept(this);
    QuadArg quadArg1 = arg;
    p->expr_2->accept(this);
    QuadArg quadArg2 = arg;
    if (quadArg1.isValue && quadArg2.isValue) {
        if (addOp == "add") {
            arg = QuadArg(true, quadArg1.value + quadArg2.value, "");
        }
        if (addOp == "sub") {
            arg = QuadArg(true, quadArg1.value - quadArg2.value, "");
        }
    } else {
        QuadAss2 *quadAss2 = new QuadAss2(ident, quadArg1, quadArg2, addOp);
        quadBlk->quadlist->push_back(quadAss2);
        if (quadArg1.type == "string") {
            arg = QuadArg(false, "", ident);
        } else {
            arg = QuadArg(false, 0, ident);
        }
    }
}

void ThreeAddressCodeConverter::visitERel(ERel *p) {
    p->relop_->accept(this);
    String relOp = op;
    Ident ident = symbolsTable->getNewSymbol();
    p->expr_1->accept(this);
    QuadArg quadArg1 = arg;
    p->expr_2->accept(this);
    QuadArg quadArg2 = arg;
    if (quadArg1.isValue && quadArg2.isValue) {
        if (relOp == "jl") {
            if (quadArg1.value < quadArg2.value) {
                arg = QuadArg(true, 1, "");
            } else {
                arg = QuadArg(true, 0, "");
            }
        }
        if (relOp == "jle") {
            if (quadArg1.value <= quadArg2.value) {
                arg = QuadArg(true, 1, "");
            } else {
                arg = QuadArg(true, 0, "");
            }
        }
        if (relOp == "jg") {
            if (quadArg1.value > quadArg2.value) {
                arg = QuadArg(true, 1, "");
            } else {
                arg = QuadArg(true, 0, "");
            }
        }
        if (relOp == "jge") {
            if (quadArg1.value >= quadArg2.value) {
                arg = QuadArg(true, 1, "");
            } else {
                arg = QuadArg(true, 0, "");
            }
        }
        if (relOp == "je") {
            if (quadArg1.value == quadArg2.value) {
                arg = QuadArg(true, 1, "");
            } else {
                arg = QuadArg(true, 0, "");
            }
        }
        if (relOp == "jne") {
            if (quadArg1.value != quadArg2.value) {
                arg = QuadArg(true, 1, "");
            } else {
                arg = QuadArg(true, 0, "");
            }
        }
    } else {
        QuadAss2 *quadAss2 = new QuadAss2(ident, quadArg1, quadArg2, relOp);
        quadBlk->quadlist->push_back(quadAss2);
        arg = QuadArg(false, 0, ident);
    }
}

void ThreeAddressCodeConverter::visitEAnd(EAnd *p) {
    Ident firstCondTrueLabel = symbolsTable->getNewSymbol();
    Ident trueLabel = symbolsTable->getNewSymbol();
    Ident falseLabel = symbolsTable->getNewSymbol();
    Ident endLabel = symbolsTable->getNewSymbol();
    Ident res = symbolsTable->getNewSymbol();
    Ident cond = symbolsTable->getNewSymbol();
    p->expr_1->accept(this);
    QuadArg quadArgTrue = QuadArg(true, 1, "");
    QuadAss2 *quadAss2 = new QuadAss2(cond, arg, quadArgTrue, "je");
    QuadArg quadArgCond1 = QuadArg(false, 1, cond);
    quadBlk->quadlist->push_back(quadAss2);
    QuadIf *quadIf = new QuadIf(quadArgCond1, firstCondTrueLabel, falseLabel);
    quadBlk->quadlist->push_back(quadIf);
    QuadLabel *quadLabelFirstCondTrue = new QuadLabel(firstCondTrueLabel);
    quadBlk->quadlist->push_back(quadLabelFirstCondTrue);
    p->expr_2->accept(this);

    QuadAss2 *quadAss22 = new QuadAss2(cond, arg, quadArgTrue, "je");
    quadBlk->quadlist->push_back(quadAss22);
    QuadIf *quadIf2 = new QuadIf(quadArgCond1, trueLabel, falseLabel);
    quadBlk->quadlist->push_back(quadIf2);

    QuadLabel *quadLabelTrue = new QuadLabel(trueLabel);
    quadBlk->quadlist->push_back(quadLabelTrue);
    QuadCopy *quadCopyTrue = new QuadCopy(res, QuadArg(true, 1, ""));
    quadBlk->quadlist->push_back(quadCopyTrue);
    QuadJmp *quadJmpEnd = new QuadJmp(endLabel, "jmp");
    quadBlk->quadlist->push_back(quadJmpEnd);
    QuadLabel *quadLabelFalse = new QuadLabel(falseLabel);
    quadBlk->quadlist->push_back(quadLabelFalse);
    QuadCopy *quadCopyFalse = new QuadCopy(res, QuadArg(true, 0, ""));
    quadBlk->quadlist->push_back(quadCopyFalse);
    QuadLabel *quadLabelEnd = new QuadLabel(endLabel);
    quadBlk->quadlist->push_back(quadLabelEnd);
    arg = QuadArg(false, 0, res);
}

void ThreeAddressCodeConverter::visitEOr(EOr *p) {
    Ident firstCondFalseLabel = symbolsTable->getNewSymbol();
    Ident trueLabel = symbolsTable->getNewSymbol();
    Ident falseLabel = symbolsTable->getNewSymbol();
    Ident endLabel = symbolsTable->getNewSymbol();
    Ident res = symbolsTable->getNewSymbol();
    Ident cond = symbolsTable->getNewSymbol();
    p->expr_1->accept(this);
    QuadArg quadArgTrue = QuadArg(true, 1, "");
    QuadAss2 *quadAss2 = new QuadAss2(cond, arg, quadArgTrue, "je");
    QuadArg quadArgCond1 = QuadArg(false, 1, cond);
    quadBlk->quadlist->push_back(quadAss2);
    QuadIf *quadIf = new QuadIf(quadArgCond1, trueLabel, firstCondFalseLabel);
    quadBlk->quadlist->push_back(quadIf);
    QuadLabel *quadLabelFirstCondFalse = new QuadLabel(firstCondFalseLabel);
    quadBlk->quadlist->push_back(quadLabelFirstCondFalse);
    p->expr_2->accept(this);

    QuadAss2 *quadAss22 = new QuadAss2(cond, arg, quadArgTrue, "je");
    quadBlk->quadlist->push_back(quadAss22);
    QuadIf *quadIf2 = new QuadIf(quadArgCond1, trueLabel, falseLabel);
    quadBlk->quadlist->push_back(quadIf2);

    QuadLabel *quadLabelTrue = new QuadLabel(trueLabel);
    quadBlk->quadlist->push_back(quadLabelTrue);
    QuadCopy *quadCopyTrue = new QuadCopy(res, QuadArg(true, 1, ""));
    quadBlk->quadlist->push_back(quadCopyTrue);
    QuadJmp *quadJmpEnd = new QuadJmp(endLabel, "jmp");
    quadBlk->quadlist->push_back(quadJmpEnd);
    QuadLabel *quadLabelFalse = new QuadLabel(falseLabel);
    quadBlk->quadlist->push_back(quadLabelFalse);
    QuadCopy *quadCopyFalse = new QuadCopy(res, QuadArg(true, 0, ""));
    quadBlk->quadlist->push_back(quadCopyFalse);
    QuadLabel *quadLabelEnd = new QuadLabel(endLabel);
    quadBlk->quadlist->push_back(quadLabelEnd);
    arg = QuadArg(false, 0, res);
}

void ThreeAddressCodeConverter::visitPlus(Plus *p) {
    op = "add";
}

void ThreeAddressCodeConverter::visitMinus(Minus *p) {
    op = "sub";
}

void ThreeAddressCodeConverter::visitTimes(Times *p) {
    op = "imul";
}

void ThreeAddressCodeConverter::visitDiv(Div *p) {
    op = "idiv";
}

void ThreeAddressCodeConverter::visitMod(Mod *p) {
    op = "mod";
}

void ThreeAddressCodeConverter::visitLTH(LTH *p) {
    op = "jl";
}

void ThreeAddressCodeConverter::visitLE(LE *p) {
    op = "jle";
}

void ThreeAddressCodeConverter::visitGTH(GTH *p) {
    op = "jg";
}

void ThreeAddressCodeConverter::visitGE(GE *p) {
    op = "jge";
}

void ThreeAddressCodeConverter::visitEQU(EQU *p) {
    op = "je";
}

void ThreeAddressCodeConverter::visitNE(NE *p) {
    op = "jne";
}

void ThreeAddressCodeConverter::visitListTopDef(ListTopDef *p) {
    BaseVisitor::visitListTopDef(p);
}

void ThreeAddressCodeConverter::visitListArg(ListArg *p) {
    BaseVisitor::visitListArg(p);
}

void ThreeAddressCodeConverter::visitListStmt(ListStmt *p) {
    BaseVisitor::visitListStmt(p);
}

void ThreeAddressCodeConverter::visitListItem(ListItem *p) {
    BaseVisitor::visitListItem(p);
}

void ThreeAddressCodeConverter::visitListType(ListType *p) {
    BaseVisitor::visitListType(p);
}

void ThreeAddressCodeConverter::visitListExpr(ListExpr *p) {
    std::vector<QuadArg> args;
    for (auto it = p->begin(); it != p->end(); it++) {
        (*it)->accept(this);
        args.push_back(arg);
    }
    for (auto it = args.rbegin(); it != args.rend(); it++) {
        QuadParam *param = new QuadParam(*(it), funIdent);
        quadBlk->quadlist->push_back(param);
    }
}

void ThreeAddressCodeConverter::visitInteger(Integer x) {
    BaseVisitor::visitInteger(x);
}

void ThreeAddressCodeConverter::visitChar(Char x) {
    BaseVisitor::visitChar(x);
}

void ThreeAddressCodeConverter::visitDouble(Double x) {
    BaseVisitor::visitDouble(x);
}

void ThreeAddressCodeConverter::visitString(String x) {
    BaseVisitor::visitString(x);
}

void ThreeAddressCodeConverter::visitIdent(Ident x) {
    BaseVisitor::visitIdent(x);
}

void ThreeAddressCodeConverter::visitNoInit(NoInit *p) {
    Ident ident = symbolsTable->getNewSymbol();
    symbolsTable->addSymbol(p->ident_, ident, declType);
    if (declType == "int") {
        QuadCopy *copy = new QuadCopy(ident, QuadArg(true, 0, ""));
        quadBlk->quadlist->push_back(copy);
    }
    if (declType == "bool") {
        QuadCopy *copy = new QuadCopy(ident, QuadArg(true, 0, ""));
        quadBlk->quadlist->push_back(copy);
    }
}

void ThreeAddressCodeConverter::visitInit(Init *p) {
    p->expr_->accept(this);
    Ident ident = symbolsTable->getNewSymbol();
    symbolsTable->addSymbol(p->ident_, ident, declType);
    QuadCopy *copy = new QuadCopy(ident, arg);
    quadBlk->quadlist->push_back(copy);
}
