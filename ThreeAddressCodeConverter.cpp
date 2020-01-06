//
// Created by szymon on 30.12.2019.
//

#include "ThreeAddressCodeConverter.h"

void ThreeAddressCodeConverter::visitFnDef(FnDef *p) {
    FunctionHeader functionHeader = functionHeaders->getHeader(p->ident_);
    symbolsTable->entryScope();
    for (std::pair<String, String> pair : functionHeader.args) {
        String newSymbol = symbolsTable->getNewSymbol();
        symbolsTable->addSymbol(pair.second, newSymbol, pair.first);
    }
    quadBlk = new QuadBlk();
    QuadFunBegin *quadFunBegin = new QuadFunBegin(p->ident_);
    quadBlk->quadlist->push_back(quadFunBegin);
    p->block_->accept(this);
    symbolsTable->exitScope();
    delete p->block_;
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
    QuadCopy *copy =  new QuadCopy(ident, arg);
    quadBlk->quadlist->push_back(copy);
}

void ThreeAddressCodeConverter::visitIncr(Incr *p) {
    Ident ident = symbolsTable->getSymbol(p->ident_);
    QuadArg arg1 = QuadArg(false, 0, ident);
    QuadArg arg2 = QuadArg(true, 1, "");
    QuadAss2 *ass = new QuadAss2(ident, arg1, arg2, "+");
    quadBlk->quadlist->push_back(ass);
}

void ThreeAddressCodeConverter::visitDecr(Decr *p) {
    Ident ident = symbolsTable->getSymbol(p->ident_);
    QuadArg arg1 = QuadArg(false, 0, ident);
    QuadArg arg2 = QuadArg(true, 1, "");
    QuadAss2 *ass = new QuadAss2(ident, arg1, arg2, "-");
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
    QuadGoto *quadGoto = new QuadGoto(endLabel);
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
    QuadGoto *quadGoto = new QuadGoto(startLabel);
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
    QuadArg q = QuadArg(false, 0, symbolsTable->getSymbol(p->ident_));
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
    p->listexpr_->accept(this);
    QuadCall *call = new QuadCall(p->ident_, p->listexpr_->size());
    quadBlk->quadlist->push_back(call);
    Ident labelIdent = symbolsTable->getNewSymbol();
    QuadLabel *quadLabel = new QuadLabel(labelIdent);
    quadBlk->quadlist->push_back(quadLabel);
    FunctionHeader functionHeader = functionHeaders->getHeader(p->ident_);
    if(functionHeader.returnType != "void"){
        Ident ident = symbolsTable->getNewSymbol();
        QuadRetrieve *quadRetrieve = new QuadRetrieve(ident);
        quadBlk->quadlist->push_back(quadRetrieve);
        arg = QuadArg(false, 0, ident);
    }
}

void ThreeAddressCodeConverter::visitEString(EString *p) {
//    Quadruple q = new Quadruple(new Ass(), )

}

void ThreeAddressCodeConverter::visitNeg(Neg *p) {
    p->expr_->accept(this);
    Ident res = symbolsTable->getNewSymbol();
    QuadAss1 *quadAss1 = new QuadAss1(res, arg, "-");
    quadBlk->quadlist->push_back(quadAss1);
    arg = QuadArg(false, 0, res);
}

void ThreeAddressCodeConverter::visitNot(Not *p) {
    p->expr_->accept(this);
    Ident res = symbolsTable->getNewSymbol();
    QuadAss1 *quadAss1 = new QuadAss1(res, arg, "~");
    quadBlk->quadlist->push_back(quadAss1);
    arg = QuadArg(false, 0, res);
}

void ThreeAddressCodeConverter::visitEMul(EMul *p) {
    p->mulop_->accept(this);
    String mulOp = op;
    Ident ident = symbolsTable->getNewSymbol();
    p->expr_1->accept(this);
    QuadArg quadArg1 = arg;
    p->expr_2->accept(this);
    QuadArg quadArg2 = arg;
    QuadAss2 *quadAss2 = new QuadAss2(ident, quadArg1, quadArg2, mulOp);
    quadBlk->quadlist->push_back(quadAss2);
    arg = QuadArg(false, 0, ident);
}

void ThreeAddressCodeConverter::visitEAdd(EAdd *p) {
    p->addop_->accept(this);
    String addOp = op;
    Ident ident = symbolsTable->getNewSymbol();
    p->expr_1->accept(this);
    QuadArg quadArg1 = arg;
    p->expr_2->accept(this);
    QuadArg quadArg2 = arg;
    QuadAss2 *quadAss2 = new QuadAss2(ident, quadArg1, quadArg2, addOp);
    quadBlk->quadlist->push_back(quadAss2);
    arg = QuadArg(false, 0, ident);
}

void ThreeAddressCodeConverter::visitERel(ERel *p) {
    p->relop_->accept(this);
    String relOp = op;
    Ident ident = symbolsTable->getNewSymbol();
    p->expr_1->accept(this);
    QuadArg quadArg1 = arg;
    p->expr_2->accept(this);
    QuadArg quadArg2 = arg;
    QuadAss2 *quadAss2 = new QuadAss2(ident, quadArg1, quadArg2, relOp);
    quadBlk->quadlist->push_back(quadAss2);
    arg = QuadArg(false, 0, ident);
}

void ThreeAddressCodeConverter::visitEAnd(EAnd *p) {
    op = "&&";
}

void ThreeAddressCodeConverter::visitEOr(EOr *p) {
    op = "||";
}

void ThreeAddressCodeConverter::visitPlus(Plus *p) {
    op = "+";
}

void ThreeAddressCodeConverter::visitMinus(Minus *p) {
    op = "-";
}

void ThreeAddressCodeConverter::visitTimes(Times *p) {
    op = "*";
}

void ThreeAddressCodeConverter::visitDiv(Div *p) {
    op = "/";
}

void ThreeAddressCodeConverter::visitMod(Mod *p) {
    op = "%";
}

void ThreeAddressCodeConverter::visitLTH(LTH *p) {
    op = "<";
}

void ThreeAddressCodeConverter::visitLE(LE *p) {
    op = "<=";
}

void ThreeAddressCodeConverter::visitGTH(GTH *p) {
    op = ">";
}

void ThreeAddressCodeConverter::visitGE(GE *p) {
    op = ">=";
}

void ThreeAddressCodeConverter::visitEQU(EQU *p) {
    op = "==";
}

void ThreeAddressCodeConverter::visitNE(NE *p) {
    op = "!=";
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
    for(auto it = p->begin(); it != p->end(); it++){
        (*it)->accept(this);
        args.push_back(arg);
    }
    for(auto it = args.rbegin(); it != args.rend(); it++){
        QuadParam *param = new QuadParam(*(it));
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
}

void ThreeAddressCodeConverter::visitInit(Init *p) {
    Ident ident = symbolsTable->getNewSymbol();
    symbolsTable->addSymbol(p->ident_, ident, declType);
    p->expr_->accept(this);
    QuadCopy *copy = new QuadCopy(ident, arg);
    quadBlk->quadlist->push_back(copy);
}
