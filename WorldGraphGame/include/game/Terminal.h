#pragma once

#include <string>

#include <termios.h>

class RawTerminal {
public:
    RawTerminal();
    ~RawTerminal();

    RawTerminal(const RawTerminal&) = delete;
    RawTerminal& operator=(const RawTerminal&) = delete;

    std::string readAvailable();

private:
    bool active_ = false;
    termios originalSettings_{};
};
