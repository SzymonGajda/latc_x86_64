//
// Created by szymon on 27.12.2019.
//

#include <iostream>
#include <map>
#include <set>
#include "FunctionHeader.h"
#include "Error.h"

FunctionHeader::FunctionHeader(Ident ident1, String returnType, std::vector<std::pair<String, String> > args,
                               int lineNumber) {
    ident = ident1;
    this->returnType = returnType;
    this->args = args;
    this->lineNumber = lineNumber;
}

FunctionHeader::FunctionHeader() {}

void FunctionHeaders::add(FunctionHeader header) {
    headers.emplace_back(header);
}

void
FunctionHeaders::add(Ident ident1, String returnType, std::vector<std::pair<String, String> > args, int lineNumber) {
    FunctionHeader functionHeader = FunctionHeader(ident1, returnType, args, lineNumber);
    headers.emplace_back(functionHeader);
}

void FunctionHeaders::print() {
    for (FunctionHeader functionHeader : headers) {
        std::cout << functionHeader.ident << " " << functionHeader.returnType << "\n";
        for (std::pair<String, String> pair : functionHeader.args) {
            std::cout << "    " << pair.first << " " << pair.second << "\n";
        }
    }
}

void FunctionHeaders::isCorrect(Error &error) {
    isHeadersUnique(error);
    for (FunctionHeader functionHeader : headers) {
        if (!error.isError) {
            isArgumentsCorrect(functionHeader, error);
        }
    }
    if (!error.isError) {
        isMainExisting(error);
    }
    if (!error.isError) {
        isMainHeaderCorrect(error);
    }
}

void FunctionHeaders::isHeadersUnique(Error &error) {
    std::set<String> set;
    for (FunctionHeader functionHeader : headers) {
        if (set.find(functionHeader.ident) != set.end()) {
            error.setIsError(true);
            error.setErrorMessage("Error at line " + std::to_string(functionHeader.lineNumber) +
                                  ": multiple declaration of function " + functionHeader.ident + "\n");
            break;
        }
        set.insert(functionHeader.ident);
    }
}

void FunctionHeaders::isArgumentsCorrect(FunctionHeader functionHeader, Error &error) {
    std::set<String> set;
    for (std::pair<String, String> pair : functionHeader.args) {
        if (pair.first == "void") {
            error.setIsError(true);
            error.setErrorMessage("Error at line " + std::to_string(functionHeader.lineNumber) +
                                  ": type of " + pair.second + " argument can't be void\n");
            break;
        }
        if (set.find(pair.second) != set.end()) {
            error.setIsError(true);
            error.setErrorMessage("Error at line " + std::to_string(functionHeader.lineNumber) +
                                  ": function can't have two arguments named " + pair.second + "\n");
            break;
        }
        set.insert(pair.second);
    }
}

void FunctionHeaders::isMainExisting(Error &error) {
    bool isExisting = false;
    for (FunctionHeader functionHeader : headers) {
        if (functionHeader.ident == "main") {
            isExisting = true;
            break;
        }
    }
    if (!isExisting) {
        error.setIsError(true);
        error.setErrorMessage("Error: declaration of main function not found\n");
    }
}

void FunctionHeaders::isMainHeaderCorrect(Error &error) {
    for (FunctionHeader functionHeader : headers) {
        if (functionHeader.ident == "main") {
            if (functionHeader.returnType != "int") {
                error.setIsError(true);
                error.setErrorMessage("Error at line " + std::to_string(functionHeader.lineNumber) +
                                      ": main function return type is " +
                                      functionHeader.returnType + " while it should be int\n");
                break;
            }
            if (!functionHeader.args.empty()) {
                error.setIsError(true);
                error.setErrorMessage("Error at line " + std::to_string(functionHeader.lineNumber) +
                                      ": main function is declared with " +
                                      std::to_string(functionHeader.args.size()) + " but it should be no-arg\n");
                break;
            }
            break;
        }
    }
}

FunctionHeaders::FunctionHeaders() {
    std::vector<std::pair<String, String> > printIntArgs;
    std::vector<std::pair<String, String> > printStringArgs;
    std::vector<std::pair<String, String> > errorArgs;
    std::vector<std::pair<String, String> > readIntArgs;
    std::vector<std::pair<String, String> > readStringArgs;
    printIntArgs.emplace_back("int", "");
    printStringArgs.emplace_back("string", "");
    add("printInt", "void", printIntArgs, 0);
    add("printString", "void", printStringArgs, 0);
    add("error", "void", errorArgs, 0);
    add("readInt", "int", readIntArgs, 0);
    add("readString", "string", readStringArgs, 0);
}

FunctionHeader FunctionHeaders::getHeader(String ident, int lineNumber, Error &error) {
       for (FunctionHeader functionHeader : headers) {
           if (functionHeader.ident == ident) {
               return functionHeader;
           }
       }

    error.setIsError(true);
    error.setErrorMessage("Error at line " + std::to_string(lineNumber) +
                          ": function " +
                           ident + " is not declared\n");
    return headers[0];
}

FunctionHeader FunctionHeaders::getHeader(String ident) {
    for (FunctionHeader functionHeader : headers) {
        if (functionHeader.ident == ident) {
            return functionHeader;
        }
    }
    return headers[0];
}




