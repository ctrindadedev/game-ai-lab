#include "game/Terminal.h"

#include <iostream>

#include <poll.h>
#include <unistd.h>

RawTerminal::RawTerminal() {
    if (!isatty(STDIN_FILENO) || tcgetattr(STDIN_FILENO, &originalSettings_) != 0) {
        return;
    }

    termios rawSettings = originalSettings_;
    rawSettings.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    rawSettings.c_cc[VMIN] = 0;
    rawSettings.c_cc[VTIME] = 0;
    active_ = (tcsetattr(STDIN_FILENO, TCSANOW, &rawSettings) == 0);

    if (active_) {
        std::cout << "\033[?25l" << std::flush;
    }
}

RawTerminal::~RawTerminal() {
    if (active_) {
        tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings_);
        std::cout << "\033[?25h" << std::flush;
    }
}

std::string RawTerminal::readAvailable() {
    std::string bytes;
    if (!active_) {
        return bytes;
    }

    pollfd descriptor{STDIN_FILENO, POLLIN, 0};
    char buffer[64];
    while (poll(&descriptor, 1, 0) > 0 && (descriptor.revents & POLLIN)) {
        const ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (count <= 0) {
            break;
        }
        bytes.append(buffer, static_cast<std::size_t>(count));
    }
    return bytes;
}
