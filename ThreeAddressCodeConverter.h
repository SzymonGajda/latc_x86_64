//
// Created by szymon on 30.12.2019.
//

#ifndef LATC_X86_64_THREEADDRESSCODECONVERTER_H
#define LATC_X86_64_THREEADDRESSCODECONVERTER_H


#include "BaseVisitor.H"
#include "FunctionHeader.h"
#include "SymbolsTable.h"

class ThreeAddressCodeConverter : public BaseVisitor {

public:

    QuadBlk *quadBlk;
    FunctionHeaders *functionHeaders;
    SymbolsTable *symbolsTable;
    std::vector<std::pair<String, String> > stringValues;

    QuadArg arg;
    int value;
    String op;
    String type;
    String declType;
    Ident funIdent;

    void visitFnDef(FnDef *p);

    void visitBlk(Blk *p);

    void visitDecl(Decl *p);

    void visitAss(Ass *p);

    void visitIncr(Incr *p);

    void visitDecr(Decr *p);

    void visitRet(Ret *p);

    void visitVRet(VRet *p);

    void visitCond(Cond *p);

    void visitCondElse(CondElse *p);

    void visitWhile(While *p);

    void visitSExp(SExp *p);

    void visitInt(Int *p);

    void visitStr(Str *p);

    void visitBool(Bool *p);

    void visitVoid(Void *p);

    void visitFun(Fun *p);

    void visitEVar(EVar *p);

    void visitELitInt(ELitInt *p);

    void visitELitTrue(ELitTrue *p);

    void visitELitFalse(ELitFalse *p);

    void visitEApp(EApp *p);

    void visitEString(EString *p);

    void visitNeg(Neg *p);

    void visitNot(Not *p);

    void visitEMul(EMul *p);

    void visitEAdd(EAdd *p);

    void visitERel(ERel *p);

    void visitEAnd(EAnd *p);

    void visitEOr(EOr *p);

    void visitPlus(Plus *p);

    void visitMinus(Minus *p);

    void visitTimes(Times *p);

    void visitDiv(Div *p);

    void visitMod(Mod *p);

    void visitLTH(LTH *p);

    void visitLE(LE *p);

    void visitGTH(GTH *p);

    void visitGE(GE *p);

    void visitEQU(EQU *p);

    void visitNE(NE *p);

    void visitListTopDef(ListTopDef *p);

    void visitListArg(ListArg *p);

    void visitListStmt(ListStmt *p);

    void visitListItem(ListItem *p);

    void visitListType(ListType *p);

    void visitListExpr(ListExpr *p);

    void visitInteger(Integer x);

    void visitChar(Char x);

    void visitDouble(Double x);

    void visitString(String x);

    void visitIdent(Ident x);

    void visitInit(Init *p);

    void visitNoInit(NoInit *p);

};


#endif //LATC_X86_64_THREEADDRESSCODECONVERTER_H
