#ifndef LATC_X86_64_TOPDEFANALYSER_H
#define LATC_X86_64_TOPDEFANALYSER_H

#include "BaseVisitor.H"
#include "FunctionHeader.h"
#include "Error.h"


class TopDefAnalyser : public BaseVisitor {

    String type;
    std::vector<std::pair<String, String> > args;


public:
    Error error;

    FunctionHeaders *functionHeaders;

    void visitProg(Prog *p);

    void visitFnDef(FnDef *p);

    void visitAr(Ar *p);

    void visitInt(Int *p);

    void visitStr(Str *p);

    void visitBool(Bool *p);

    void visitVoid(Void *p);
};

#endif //LATC_X86_64_TOPDEFANALYSER_H
