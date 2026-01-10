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
    void sendWarning(char *msg);
    void send(char *msg);
    void sendError(char *msg, bool kill = 1);

    Log() = default;
};
