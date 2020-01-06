//
// Created by szymon on 27.12.2019.
//

#include "TopDefAnalyser.H"


void TopDefAnalyser::visitProg(Prog *p) {
    p->listtopdef_->accept(this);
    functionHeaders->isCorrect(error);
}

void TopDefAnalyser::visitFnDef(FnDef *fndef) {

    fndef->listarg_->accept(this);
    fndef->type_->accept(this);
    functionHeaders->add(fndef->ident_, type, args, fndef->line_number);
    args.clear();
}

void TopDefAnalyser::visitAr(Ar *ar) {

    ar->type_->accept(this);
    args.emplace_back(std::make_pair(type, ar->ident_));
}

void TopDefAnalyser::visitInt(Int *intVal) {
    type = "int";
}

void TopDefAnalyser::visitStr(Str *str) {
    type = "string";
}

void TopDefAnalyser::visitBool(Bool *boolVal) {
    type = "boolean";
}

void TopDefAnalyser::visitVoid(Void *voidVal) {
    type = "void";
}


