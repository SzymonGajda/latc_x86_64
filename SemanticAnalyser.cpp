//
// Created by szymon on 28.12.2019.
//

#include <iostream>
#include "SemanticAnalyser.h"

void SemanticAnalyser::visitProg(Prog *prog) {

    prog->listtopdef_->accept(this);
}

void SemanticAnalyser::visitFnDef(FnDef *fndef) {

    FunctionHeader functionHeader = functionHeaders->getHeader(fndef->ident_, fndef->line_number, *error);
    isReturn = false;
    if (!error->isError) {
        symbolsTable->entryScope();
        for (std::pair<String, String> pair : functionHeader.args) {
            symbolsTable->addSymbol(pair.second, pair.first, fndef->line_number, *error);
        }
        returnType = functionHeader.returnType;
        fndef->block_->accept(this);
        symbolsTable->exitScope();
        if (!error->isError) {
            if (functionHeader.returnType != "void" && isReturn == false) {
                error->setIsError(true);
                error->setErrorMessage("Error at line " + std::to_string(fndef->line_number) +
                                       ": function " + fndef->ident_ + " is declared with return type " +
                                       functionHeader.returnType +
                                       " but there exists execution paths without return statement\n");
            }
        }
    }
}

void SemanticAnalyser::visitAr(Ar *ar) {

}

void SemanticAnalyser::visitBlk(Blk *blk) {

    symbolsTable->entryScope();
    blk->liststmt_->accept(this);
    symbolsTable->exitScope();
}

void SemanticAnalyser::visitEmpty(Empty *empty) {

}

void SemanticAnalyser::visitBStmt(BStmt *bstmt) {

    bstmt->block_->accept(this);

}

void SemanticAnalyser::visitDecl(Decl *decl) {

    decl->type_->accept(this);
    if (type == "void") {
        error->setIsError(true);
        error->setErrorMessage("Error at line " + std::to_string(decl->line_number) +
                               ": variables can't have void type\n");
    } else {
        DeclVisitor declVisitor;
        declVisitor.declType = type;
        declVisitor.functionHeaders = functionHeaders;
        declVisitor.symbolsTable = symbolsTable;
        declVisitor.error = error;
        declVisitor.isReturn = isReturn;
        decl->listitem_->accept(&declVisitor);
    }
}

void SemanticAnalyser::visitAss(Ass *ass) {

    String varType = symbolsTable->getType(ass->ident_, ass->line_number, *error);
    if (!error->isError) {
        ass->expr_->accept(this);
    }
    if (!error->isError) {
        if (type != varType) {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(ass->line_number) +
                                   ": variable has type " + varType +
                                   " while assigned expression is of type " + type + "\n");
        }
    }
}

void SemanticAnalyser::visitIncr(Incr *incr) {
    /* Code For Incr Goes Here */
    String varType = symbolsTable->getType(incr->ident_, incr->line_number, *error);
    if (!error->isError) {
        if (varType != "int") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(incr->line_number) +
                                   ": variable has type " + varType +
                                   " but only int variables can be incremented\n");
        }
    }

}

void SemanticAnalyser::visitDecr(Decr *decr) {

    String varType = symbolsTable->getType(decr->ident_, decr->line_number, *error);
    if (!error->isError) {
        if (varType != "int") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(decr->line_number) +
                                   ": variable has type " + varType +
                                   " but only int variables can be decremented\n");
        }
    }
}

void SemanticAnalyser::visitRet(Ret *ret) {
    /* Code For Ret Goes Here */
    if (returnType == "void") {
        error->setIsError(true);
        error->setErrorMessage("Error at line " + std::to_string(ret->line_number) +
                               ": this function return type is void so it can't return any value\n");
        return;
    }
    ret->expr_->accept(this);
    if (!error->isError) {
        if (type != returnType) {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(ret->line_number) +
                                   ": this function must return values of type " + returnType +
                                   " but actual type of return expression is " + type + "\n");
            return;
        }
    }
    isReturn = true;
}

void SemanticAnalyser::visitVRet(VRet *vret) {
    /* Code For VRet Goes Here */
    if (returnType != "void") {
        error->setIsError(true);
        error->setErrorMessage("Error at line " + std::to_string(vret->line_number) +
                               ": this function must return values of type " + returnType +
                               " but this statement returns no value\n");
    }
}

void SemanticAnalyser::visitCond(Cond *cond) {
    /* Code For Cond Goes Here */

    bool isReturnPre = isReturn;
    bool condValueSet = false;
    bool condValueTrue = false;

    cond->expr_->accept(this);
    if (!error->isError) {
        if (type != "boolean") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(cond->line_number) +
                                   ": condition in if statement must be boolean instead of " + type + "\n");
            return;
        }
        condValueSet = isConditionValueSet;
        condValueTrue = isConditionTrue;
        cond->stmt_->accept(this);
        if (condValueSet && condValueTrue && isReturn) {
            isReturn = true;
        } else {
            isReturn = isReturnPre;
        }
    }
}

void SemanticAnalyser::visitCondElse(CondElse *condelse) {
    /* Code For CondElse Goes Here */

    bool isReturnPre = isReturn;
    bool condValueSet = false;
    bool condValueTrue = false;
    bool isReturn1 = false;
    bool isReturn2 = false;

    condelse->expr_->accept(this);
    if (!error->isError) {
        if (type != "boolean") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(condelse->line_number) +
                                   ": condition in if statement must be boolean instead of " + type + "\n");
            return;
        }
        condValueSet = isConditionValueSet;
        condValueTrue = isConditionTrue;
        condelse->stmt_1->accept(this);
        isReturn1 = isReturn;
        if (!error->isError) {
            condelse->stmt_2->accept(this);
            isReturn2 = isReturn;
            isReturn = isReturnPre;
            if (condValueSet) {
                if (condValueTrue && isReturn1) {
                    isReturn = true;
                }
                if (!condValueTrue && isReturn2) {
                    isReturn = true;
                }
            } else {
                if (isReturn1 && isReturn2) {
                    isReturn = true;
                }
            }

        }
    }
}

void SemanticAnalyser::visitWhile(While *whileStmt) {
    /* Code For While Goes Here */
    bool isReturnPre = isReturn;
    bool condValueSet = false;
    bool condValueTrue = false;
    whileStmt->expr_->accept(this);
    if (!error->isError) {
        if (type != "boolean") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(whileStmt->line_number) +
                                   ": condition in while must be boolean instead of " + type + "\n");
            return;
        }
        condValueSet = isConditionValueSet;
        condValueTrue = isConditionTrue;
        whileStmt->stmt_->accept(this);
        if (condValueSet && condValueTrue && isReturn) {
            isReturn = true;
        } else {
            isReturn = isReturnPre;
        }
    }

}

void SemanticAnalyser::visitSExp(SExp *sexp) {
    /* Code For SExp Goes Here */
    sexp->expr_->accept(this);
}


void SemanticAnalyser::visitInt(Int *intVal) {
    /* Code For Int Goes Here */
    type = "int";
}

void SemanticAnalyser::visitStr(Str *str) {
    /* Code For Str Goes Here */
    type = "string";
}

void SemanticAnalyser::visitBool(Bool *boolVal) {
    /* Code For Bool Goes Here */
    type = "boolean";
}

void SemanticAnalyser::visitVoid(Void *voidVal) {
    /* Code For Void Goes Here */
    type = "void";
}

void SemanticAnalyser::visitFun(Fun *fun) {
    /* Code For Fun Goes Here */
    fun->type_->accept(this);
}

void SemanticAnalyser::visitEVar(EVar *evar) {
    /* Code For EVar Goes Here */

    type = symbolsTable->getType(evar->ident_, evar->line_number, *error);
    isConditionValueSet = false;
}

void SemanticAnalyser::visitELitInt(ELitInt *elitint) {
    /* Code For ELitInt Goes Here */
    type = "int";
}

void SemanticAnalyser::visitELitTrue(ELitTrue *elittrue) {
    /* Code For ELitTrue Goes Here */
    type = "boolean";
    isConditionValueSet = true;
    isConditionTrue = true;
}

void SemanticAnalyser::visitELitFalse(ELitFalse *elitfalse) {
    /* Code For ELitFalse Goes Here */
    type = "boolean";
    isConditionValueSet = true;
    isConditionTrue = false;
}

void SemanticAnalyser::visitEApp(EApp *eapp) {
    /* Code For EApp Goes Here */
    FunctionHeader functionHeader = functionHeaders->getHeader(eapp->ident_, eapp->line_number, *error);
    type = functionHeader.returnType;
    if (!error->isError) {
        if (eapp->listexpr_->size() != functionHeader.args.size()) {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(eapp->line_number) +
                                   ": function " + eapp->ident_ + " was declared with " +
                                   std::to_string(functionHeader.args.size()) +
                                   " argument(s) so it cannot be applied to " +
                                   std::to_string(eapp->listexpr_->size()) + " argument(s)\n");
            return;
        }
        int ind = 0;
        for (ListExpr::iterator i = eapp->listexpr_->begin(); i != eapp->listexpr_->end(); ++i) {
            if (!error->isError) {
                (*i)->accept(this);
            }
            if (!error->isError) {
                if (functionHeader.args[ind].first != type) {
                    error->setIsError(true);
                    error->setErrorMessage("Error at line " + std::to_string(eapp->line_number) +
                                           ": function " + eapp->ident_ + " was declared with argument num " +
                                           std::to_string(ind) + " of type " + functionHeader.args[ind].first +
                                           " but actual one is of type " + type + "\n");
                    return;
                }
            }
            ind++;
        }
        type = functionHeader.returnType;
        isConditionValueSet = false;
    }
}

void SemanticAnalyser::visitEString(EString *estring) {
    /* Code For EString Goes Here */
    type = "string";
}

void SemanticAnalyser::visitNeg(Neg *neg) {
    /* Code For Neg Goes Here */
    neg->expr_->accept(this);
    if (!error->isError) {
        if (type != "int") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(neg->line_number) +
                                   ": negation can be only applied to int expressions while this is of type " + type +
                                   "\n");
            return;
        }
        type = "int";
    }
}

void SemanticAnalyser::visitNot(Not *notOp) {
    /* Code For Not Goes Here */

    notOp->expr_->accept(this);
    if (!error->isError) {
        if (type != "boolean") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(notOp->line_number) +
                                   ": not can be only applied to boolean expressions while this is of type " + type +
                                   "\n");
            return;
        }
        type = "boolean";
        isConditionValueSet = false;
    }

}

void SemanticAnalyser::visitEMul(EMul *emul) {
    /* Code For EMul Goes Here */
    emul->expr_1->accept(this);
    String type1 = type;
    String type2;
    if (!error->isError) {
        emul->expr_2->accept(this);
        type2 = type;
    }
    if (!error->isError) {
        emul->mulop_->accept(this);
        if (type1 != "int" || type2 != "int") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(emul->line_number) +
                                   ": both arguments of " + op + " operation must be int\n");
            return;
        }
        type = "int";
    }
}

void SemanticAnalyser::visitEAdd(EAdd *eadd) {
    /* Code For EAdd Goes Here */

    eadd->expr_1->accept(this);
    String type1 = type;
    String type2;
    if (!error->isError) {
        eadd->expr_2->accept(this);
        type2 = type;
    }
    if (!error->isError) {
        eadd->addop_->accept(this);
        if (!((type1 == "int" && type2 == "int") || (type1 == "string" && type2 == "string"))) {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(eadd->line_number) +
                                   ": both arguments of " + op + " operation must be int or string\n");
            return;
        }
        if (type1 == "string" && type2 == "string" && op != "+") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(eadd->line_number) +
                                   ": operation " + op + " can't be applied to string values\n");
            return;
        }
        if (type1 == "int") {
            type = "int";
        } else {
            type = "string";
        }
    }

}

void SemanticAnalyser::visitERel(ERel *erel) {
    /* Code For ERel Goes Here */

    erel->expr_1->accept(this);
    String type1 = type;
    String type2;
    if (!error->isError) {
        erel->expr_2->accept(this);
        type2 = type;
    }
    if (!error->isError) {
        erel->relop_->accept(this);
        if (op == "==" || op == "!=") {
            if (type1 != type2) {
                error->setIsError(true);
                error->setErrorMessage("Error at line " + std::to_string(erel->line_number) +
                                       ": both arguments of " + op + " operation must be of same type" +
                                       "\n");
                return;
            }
        } else {
            if (type1 != "int" || type2 != "int") {
                error->setIsError(true);
                error->setErrorMessage("Error at line " + std::to_string(erel->line_number) +
                                       ": both arguments of " + op + " operation must be int" +
                                       "\n");
                return;
            }
        }
        type = "boolean";
        isConditionValueSet = false;
    }

}

void SemanticAnalyser::visitEAnd(EAnd *eand) {
    /* Code For EAnd Goes Here */

    eand->expr_1->accept(this);
    String type1 = type;
    String type2;
    if (!error->isError) {
        eand->expr_2->accept(this);
        type2 = type;
    }
    if (!error->isError) {
        if (type1 != "boolean" || type2 != "boolean") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(eand->line_number) +
                                   ": both arguments of && operation must be boolean\n");
            return;
        }
        type = "boolean";
        isConditionValueSet = false;
    }

}

void SemanticAnalyser::visitEOr(EOr *eor) {
    /* Code For EOr Goes Here */

    eor->expr_1->accept(this);
    String type1 = type;
    String type2;
    if (!error->isError) {
        eor->expr_2->accept(this);
        type2 = type;
    }
    if (!error->isError) {
        if (type1 != "boolean" || type2 != "boolean") {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(eor->line_number) +
                                   ": both arguments of || operation must be boolean\n");
            return;
        }
        type = "boolean";
        isConditionValueSet = false;
    }

}

void SemanticAnalyser::visitPlus(Plus *plus) {
    /* Code For Plus Goes Here */
    op = "+";
}

void SemanticAnalyser::visitMinus(Minus *minus) {
    /* Code For Minus Goes Here */
    op = "-";
}

void SemanticAnalyser::visitTimes(Times *times) {
    /* Code For Times Goes Here */
    op = "*";
}

void SemanticAnalyser::visitDiv(Div *div) {
    /* Code For Div Goes Here */
    op = "/";
}

void SemanticAnalyser::visitMod(Mod *mod) {
    /* Code For Mod Goes Here */
    op = "%";
}

void SemanticAnalyser::visitLTH(LTH *lth) {
    /* Code For LTH Goes Here */
    op = "<";
}

void SemanticAnalyser::visitLE(LE *le) {
    /* Code For LE Goes Here */
    op = "<=";
}

void SemanticAnalyser::visitGTH(GTH *gth) {
    /* Code For GTH Goes Here */
    op = ">";
}

void SemanticAnalyser::visitGE(GE *ge) {
    /* Code For GE Goes Here */
    op = ">=";
}

void SemanticAnalyser::visitEQU(EQU *equ) {
    /* Code For EQU Goes Here */
    op = "==";
}

void SemanticAnalyser::visitNE(NE *ne) {
    /* Code For NE Goes Here */
    op = "!=";
}


void SemanticAnalyser::visitListTopDef(ListTopDef *listtopdef) {
    for (ListTopDef::iterator i = listtopdef->begin(); i != listtopdef->end(); ++i) {
        if (!error->isError) {
            (*i)->accept(this);
        }
    }
}

void SemanticAnalyser::visitListArg(ListArg *listarg) {
    for (ListArg::iterator i = listarg->begin(); i != listarg->end(); ++i) {
        if (!error->isError) {
            (*i)->accept(this);
        }
    }
}

void SemanticAnalyser::visitListStmt(ListStmt *liststmt) {
    for (ListStmt::iterator i = liststmt->begin(); i != liststmt->end(); ++i) {
        if (!error->isError) {
            (*i)->accept(this);
        }
    }
}

void SemanticAnalyser::visitListItem(ListItem *listitem) {
    for (ListItem::iterator i = listitem->begin(); i != listitem->end(); ++i) {
        if (!error->isError) {
            (*i)->accept(this);
        }
    }
}

void SemanticAnalyser::visitListType(ListType *listtype) {
    for (ListType::iterator i = listtype->begin(); i != listtype->end(); ++i) {
        if (!error->isError) {
            (*i)->accept(this);
        }
    }
}

void SemanticAnalyser::visitListExpr(ListExpr *listexpr) {
    for (ListExpr::iterator i = listexpr->begin(); i != listexpr->end(); ++i) {
        if (!error->isError) {
            (*i)->accept(this);
        }
    }
}


void SemanticAnalyser::visitInteger(Integer x) {
    /* Code for Integer Goes Here */
}

void SemanticAnalyser::visitChar(Char x) {
    /* Code for Char Goes Here */
}

void SemanticAnalyser::visitDouble(Double x) {
    /* Code for Double Goes Here */
}

void SemanticAnalyser::visitString(String x) {
    /* Code for String Goes Here */
}

void SemanticAnalyser::visitIdent(Ident x) {
    /* Code for Ident Goes Here */
}


void DeclVisitor::visitNoInit(NoInit *p) {
    symbolsTable->addSymbol(p->ident_, declType, p->line_number, *error);
}

void DeclVisitor::visitInit(Init *p) {
    p->expr_->accept(this);
    if (!error->isError) {
        if (type != declType) {
            error->setIsError(true);
            error->setErrorMessage("Error at line " + std::to_string(p->line_number) +
                                   ": declared variable has type " + declType +
                                   " while initialization expression is of type " + type + "\n");
            return;
        }
        symbolsTable->addSymbol(p->ident_, declType, p->line_number, *error);
    }
}
