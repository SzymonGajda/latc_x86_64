//
// Created by szymon on 28.12.2019.
//

#include "Error.h"

Error::Error(bool isError, const String &errorMessage) : isError(isError), errorMessage(errorMessage) {}

bool Error::isError1() const {
    return isError;
}

void Error::setIsError(bool isError) {
    Error::isError = isError;
}

const String &Error::getErrorMessage() const {
    return errorMessage;
}

void Error::setErrorMessage(const String &errorMessage) {
    Error::errorMessage = errorMessage;
}

Error::Error() {
    isError = false;
}
