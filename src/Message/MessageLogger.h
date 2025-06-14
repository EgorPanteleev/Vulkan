//
// Created by auser on 4/4/25.
//

#ifndef VULKAN_MESSAGELOGGER_H
#define VULKAN_MESSAGELOGGER_H


#include <iostream>

namespace TerminalColors {
    static constexpr auto RED     = "\033[31m";
    static constexpr auto GREEN   = "\033[32m";
    static constexpr auto BLUE    = "\033[34m";
    static constexpr auto CYAN    = "\033[36m";
    static constexpr auto WHITE   = "\033[37m";
    static constexpr auto GRAY    = "\033[90m";
    static constexpr auto YELLOW  = "\033[33m";
    static constexpr auto PINK    = "\033[95m";
    static constexpr auto PURPLE  = "\033[35m";
}

/**
* Wrapper for console output
*/

class MessageLogger {
public:
    const std::string reset = "\033[0m";

    MessageLogger( std::ostream& os, const std::string& color, bool autoEndOfLine = true );

    ~MessageLogger();

    MessageLogger(const MessageLogger&) = delete;
    MessageLogger& operator=(const MessageLogger&) = delete;

    MessageLogger(MessageLogger&&) = delete;
    MessageLogger& operator=(MessageLogger&&) = delete;

    template <typename T>
    MessageLogger& operator<<(T&& value) {
        os << color << std::forward<T>(value) << reset;
        return *this;
    }

private:
    std::ostream& os;
    std::string color;
    bool autoEndOfLine;
};


#define MESSAGE  MessageLogger( std::cout, TerminalColors::GRAY   )
#define INFO     MessageLogger( std::cout, TerminalColors::CYAN   )
#define DEBUG    MessageLogger( std::cout, TerminalColors::BLUE   )
#define WARNING  MessageLogger( std::cout, TerminalColors::YELLOW )
#define ERROR    MessageLogger( std::cerr, TerminalColors::RED    )


#endif //VULKAN_MESSAGELOGGER_H
