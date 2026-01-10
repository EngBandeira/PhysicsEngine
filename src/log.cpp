#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include "log.hpp"

void Terminal::update(unsigned int index){
    char *buffer = lines[index];
    unsigned int lenght = strlen(buffer);
    std::vector<char*> args;

    unsigned int i = 0;
    while(i < lenght) {

        while(buffer[i] == ' ') i++;
        unsigned int k = i;

        while(buffer[i] != ' ' && buffer[i] != 0) i++;
        char *p = (char*) malloc(i - k + 1);

        memcpy(p, buffer + k, i-k);
        p[i-k] = 0;
        args.push_back(p);
    }
}

void Terminal::update() {
    update(lines.size()-1);
}

void Log::sendWarning(char *msg) {
    printf("WARNING: %s\n",msg);
    unsigned int lenght = strlen(msg) + 10;

    char *p = (char*) malloc(lenght + 1);
    sprintf(p, "WARNING: %s", msg);

    p[lenght] = 0;
    terminal.lines.push_back(p);
}

void Log::send(char *msg) {
    printf("%s\n",msg);
    unsigned int lenght = strlen(msg);
    char *p = (char*) malloc(lenght + 1);
    memcpy(p, msg, lenght);

    p[lenght] = 0;
    terminal.lines.push_back(p);
}

void Log::sendError(char *msg, bool kill) {
    fprintf(stderr, "\n\nError: %s\n\n", msg);

    if( kill ) exit(EXIT_FAILURE);

    unsigned int lenght = strlen(msg) + 8;
    char *p = (char*) malloc(lenght + 1);
    sprintf(p, "ERROR: %s", msg);

    p[lenght] = 0;
    terminal.lines.push_back(p);
}
