/*** BNFC-Generated Visitor Design Pattern Skeleton. ***/
/* This implements the common visitor design pattern.
   Note that this method uses Visitor-traversal of lists, so
   List->accept() does NOT traverse the list. This allows different
   algorithms to use context information differently. */

#include "BaseVisitor.H"


void BaseVisitor::visitProgram(Program *t) {} //abstract class
void BaseVisitor::visitTopDef(TopDef *t) {} //abstract class
void BaseVisitor::visitArg(Arg *t) {} //abstract class
void BaseVisitor::visitBlock(Block *t) {} //abstract class
void BaseVisitor::visitStmt(Stmt *t) {} //abstract class
void BaseVisitor::visitItem(Item *t) {} //abstract class
void BaseVisitor::visitType(Type *t) {} //abstract class
void BaseVisitor::visitExpr(Expr *t) {} //abstract class
void BaseVisitor::visitAddOp(AddOp *t) {} //abstract class
void BaseVisitor::visitMulOp(MulOp *t) {} //abstract class
void BaseVisitor::visitRelOp(RelOp *t) {} //abstract class

void BaseVisitor::visitProg(Prog *prog) {

    prog->listtopdef_->accept(this);
}

void BaseVisitor::visitFnDef(FnDef *fndef) {

    fndef->type_->accept(this);
    visitIdent(fndef->ident_);
    fndef->listarg_->accept(this);
    fndef->block_->accept(this);
}

void BaseVisitor::visitAr(Ar *ar) {

    ar->type_->accept(this);
    visitIdent(ar->ident_);
}

void BaseVisitor::visitBlk(Blk *blk) {

    blk->liststmt_->accept(this);

}

void BaseVisitor::visitEmpty(Empty *empty) {


}

void BaseVisitor::visitBStmt(BStmt *bstmt) {

    bstmt->block_->accept(this);

}

void BaseVisitor::visitDecl(Decl *decl) {

    decl->type_->accept(this);
    decl->listitem_->accept(this);

}

void BaseVisitor::visitAss(Ass *ass) {

    visitIdent(ass->ident_);
    ass->expr_->accept(this);

}

void BaseVisitor::visitIncr(Incr *incr) {
    /* Code For Incr Goes Here */

    visitIdent(incr->ident_);

}

void BaseVisitor::visitDecr(Decr *decr) {
    /* Code For Decr Goes Here */

    visitIdent(decr->ident_);

}

void BaseVisitor::visitRet(Ret *ret) {
    /* Code For Ret Goes Here */

    ret->expr_->accept(this);

}

void BaseVisitor::visitVRet(VRet *vret) {
    /* Code For VRet Goes Here */


}

void BaseVisitor::visitCond(Cond *cond) {
    /* Code For Cond Goes Here */

    cond->expr_->accept(this);
    cond->stmt_->accept(this);

}

void BaseVisitor::visitCondElse(CondElse *condelse) {
    /* Code For CondElse Goes Here */

    condelse->expr_->accept(this);
    condelse->stmt_1->accept(this);
    condelse->stmt_2->accept(this);

}

void BaseVisitor::visitWhile(While *whileStmt) {
    /* Code For While Goes Here */

    whileStmt->expr_->accept(this);
    whileStmt->stmt_->accept(this);

}

void BaseVisitor::visitSExp(SExp *sexp) {
    /* Code For SExp Goes Here */

    sexp->expr_->accept(this);

}

void BaseVisitor::visitNoInit(NoInit *noinit) {
    /* Code For NoInit Goes Here */

    visitIdent(noinit->ident_);

}

void BaseVisitor::visitInit(Init *init) {
    /* Code For Init Goes Here */

    visitIdent(init->ident_);
    init->expr_->accept(this);

}

void BaseVisitor::visitInt(Int *intVal) {
    /* Code For Int Goes Here */


}

void BaseVisitor::visitStr(Str *str) {
    /* Code For Str Goes Here */


}

void BaseVisitor::visitBool(Bool *boolVal) {
    /* Code For Bool Goes Here */


}

void BaseVisitor::visitVoid(Void *voidVal) {
    /* Code For Void Goes Here */


}

void BaseVisitor::visitFun(Fun *fun) {
    /* Code For Fun Goes Here */

    fun->type_->accept(this);
    fun->listtype_->accept(this);

}

void BaseVisitor::visitEVar(EVar *evar) {
    /* Code For EVar Goes Here */

    visitIdent(evar->ident_);

}

void BaseVisitor::visitELitInt(ELitInt *elitint) {
    /* Code For ELitInt Goes Here */

    visitInteger(elitint->integer_);

}

void BaseVisitor::visitELitTrue(ELitTrue *elittrue) {
    /* Code For ELitTrue Goes Here */


}

void BaseVisitor::visitELitFalse(ELitFalse *elitfalse) {
    /* Code For ELitFalse Goes Here */


}

void BaseVisitor::visitEApp(EApp *eapp) {
    /* Code For EApp Goes Here */

    visitIdent(eapp->ident_);
    eapp->listexpr_->accept(this);

}

void BaseVisitor::visitEString(EString *estring) {
    /* Code For EString Goes Here */

    visitString(estring->string_);

}

void BaseVisitor::visitNeg(Neg *neg) {
    /* Code For Neg Goes Here */

    neg->expr_->accept(this);

}

void BaseVisitor::visitNot(Not *notOp) {
    /* Code For Not Goes Here */

    notOp->expr_->accept(this);

}

void BaseVisitor::visitEMul(EMul *emul) {
    /* Code For EMul Goes Here */

    emul->expr_1->accept(this);
    emul->mulop_->accept(this);
    emul->expr_2->accept(this);

}

void BaseVisitor::visitEAdd(EAdd *eadd) {
    /* Code For EAdd Goes Here */

    eadd->expr_1->accept(this);
    eadd->addop_->accept(this);
    eadd->expr_2->accept(this);

}

void BaseVisitor::visitERel(ERel *erel) {
    /* Code For ERel Goes Here */

    erel->expr_1->accept(this);
    erel->relop_->accept(this);
    erel->expr_2->accept(this);

}

void BaseVisitor::visitEAnd(EAnd *eand) {
    /* Code For EAnd Goes Here */

    eand->expr_1->accept(this);
    eand->expr_2->accept(this);

}

void BaseVisitor::visitEOr(EOr *eor) {
    /* Code For EOr Goes Here */

    eor->expr_1->accept(this);
    eor->expr_2->accept(this);

}

void BaseVisitor::visitPlus(Plus *plus) {
    /* Code For Plus Goes Here */


}

void BaseVisitor::visitMinus(Minus *minus) {
    /* Code For Minus Goes Here */


}

void BaseVisitor::visitTimes(Times *times) {
    /* Code For Times Goes Here */


}

void BaseVisitor::visitDiv(Div *div) {
    /* Code For Div Goes Here */


}

void BaseVisitor::visitMod(Mod *mod) {
    /* Code For Mod Goes Here */


}

void BaseVisitor::visitLTH(LTH *lth) {
    /* Code For LTH Goes Here */


}

void BaseVisitor::visitLE(LE *le) {
    /* Code For LE Goes Here */


}

void BaseVisitor::visitGTH(GTH *gth) {
    /* Code For GTH Goes Here */


}

void BaseVisitor::visitGE(GE *ge) {
    /* Code For GE Goes Here */


}

void BaseVisitor::visitEQU(EQU *equ) {
    /* Code For EQU Goes Here */


}

void BaseVisitor::visitNE(NE *ne) {
    /* Code For NE Goes Here */


}


void BaseVisitor::visitListTopDef(ListTopDef *listtopdef) {
    for (ListTopDef::iterator i = listtopdef->begin(); i != listtopdef->end(); ++i) {
        (*i)->accept(this);
    }
}

void BaseVisitor::visitListArg(ListArg *listarg) {
    for (ListArg::iterator i = listarg->begin(); i != listarg->end(); ++i) {
        (*i)->accept(this);
    }
}

void BaseVisitor::visitListStmt(ListStmt *liststmt) {
    for (ListStmt::iterator i = liststmt->begin(); i != liststmt->end(); ++i) {
        (*i)->accept(this);
    }
}

void BaseVisitor::visitListItem(ListItem *listitem) {
    for (ListItem::iterator i = listitem->begin(); i != listitem->end(); ++i) {
        (*i)->accept(this);
    }
}

void BaseVisitor::visitListType(ListType *listtype) {
    for (ListType::iterator i = listtype->begin(); i != listtype->end(); ++i) {
        (*i)->accept(this);
    }
}

void BaseVisitor::visitListExpr(ListExpr *listexpr) {
    for (ListExpr::iterator i = listexpr->begin(); i != listexpr->end(); ++i) {
        (*i)->accept(this);
    }
}


void BaseVisitor::visitInteger(Integer x) {
    /* Code for Integer Goes Here */
}

void BaseVisitor::visitChar(Char x) {
    /* Code for Char Goes Here */
}

void BaseVisitor::visitDouble(Double x) {
    /* Code for Double Goes Here */
}

void BaseVisitor::visitString(String x) {
    /* Code for String Goes Here */
}

void BaseVisitor::visitIdent(Ident x) {
    /* Code for Ident Goes Here */
}

void BaseVisitor::visitQuadruple(Quadruple *q) {
    q->accept(this);
}

void BaseVisitor::visitQuadBlk(QuadBlk *q) {

}

void BaseVisitor::visitQuadAss1(QuadAss1 *q) {

}

void BaseVisitor::visitQuadAss2(QuadAss2 *q) {

}

void BaseVisitor::visitQuadCopy(QuadCopy *q) {

}

void BaseVisitor::visitQuadGoto(QuadGoto *q) {

}

void BaseVisitor::visitQuadLabel(QuadLabel *q) {

}

void BaseVisitor::visitQuadIf(QuadIf *q) {

}

void BaseVisitor::visitQuadParam(QuadParam *q) {

}

void BaseVisitor::visitQuadCall(QuadCall *q) {

}

void BaseVisitor::visitQuadReturn(QuadReturn *q) {

}

void BaseVisitor::visitQuadArg(QuadArg *q) {

}

void BaseVisitor::visitQuadReturnNoVal(QuadReturnNoVal *q) {

}

void BaseVisitor::visitQuadRetrieve(QuadRetrieve *q) {

}

void BaseVisitor::visitQuadFunBegin(QuadFunBegin *q) {

}



