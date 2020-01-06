//
// Created by szymon on 28.12.2019.
//

#ifndef LATC_X86_64_ERROR_H
#define LATC_X86_64_ERROR_H


#include "Absyn.H"

class Error {

public:
    bool isError;
    String errorMessage;

    Error(bool isError, const String &errorMessage);

    Error();

    bool isError1() const;

    void setIsError(bool isError);

    const String &getErrorMessage() const;

    void setErrorMessage(const String &errorMessage);

};


#endif //LATC_X86_64_ERROR_H
