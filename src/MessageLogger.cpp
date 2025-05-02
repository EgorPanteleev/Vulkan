//
// Created by auser on 4/4/25.
//

#include "MessageLogger.h"

MessageLogger::MessageLogger(std::ostream& os, const std::string& color, bool autoEndOfLine ):
                             os(os), color(color), autoEndOfLine(autoEndOfLine) {
}

MessageLogger::~MessageLogger() {
    if (autoEndOfLine) std::cout << std::endl;
}
