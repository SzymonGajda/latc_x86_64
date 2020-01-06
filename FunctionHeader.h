//
// Created by szymon on 27.12.2019.
//

#ifndef LATC_X86_64_FUNCTIONHEADER_H
#define LATC_X86_64_FUNCTIONHEADER_H

#include "Absyn.H"
#include "Error.h"


class FunctionHeader {

public:
    FunctionHeader(Ident ident1, String returnType, std::vector<std::pair<String, String> > args, int lineNumber);

    FunctionHeader();

    Ident ident;
    String returnType;
    int lineNumber;
    std::vector<std::pair<String, String> > args;
};

class FunctionHeaders {

    void isHeadersUnique(Error &error);

    void isMainExisting(Error &error);

    void isMainHeaderCorrect(Error &error);

    void isArgumentsCorrect(FunctionHeader functionHeader, Error &error);

public:
    std::vector<FunctionHeader> headers;

    FunctionHeaders();

    void add(FunctionHeader header);

    void add(Ident ident1, String returnType, std::vector<std::pair<String, String> > args, int lineNumber);

    void print();

    void isCorrect(Error &error);

    FunctionHeader getHeader(String ident, int lineNumber, Error &error);

    FunctionHeader getHeader(String ident);
};


#endif //LATC_X86_64_FUNCTIONHEADER_H
