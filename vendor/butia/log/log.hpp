#pragma once
#include <vector>

struct Terminal {
    std::vector<char*> lines;
    void update(unsigned int index);
    void update();

    Terminal() = default;
};


class Log {
public:
    Terminal terminal;
    void sendWarning(const char *msg);
    void send(const char *msg);
    void sendError(const char *msg, bool kill = 1);

    Log() = default;
};
