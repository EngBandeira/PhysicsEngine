
#include "utils.hpp"
#include "render.hpp"
#include <stdio.h>

char *readFile(const char *localPath, unsigned int *fileLenght) {
    FILE *file = fopen (localPath, "r");
    char *buffer;
    if (file == NULL){
        char message[100];
        sprintf(message, "cannot open file %s", localPath);
        sendError(message);
        exit (1);
    }
    fseek (file, 0L, SEEK_END);
    unsigned int sz = ftell (file) + 1;
    fseek (file, 0, SEEK_SET);
    buffer = (char *)malloc (sz * sizeof (char));
    buffer[sz - 2] = EOF;
    fread (buffer, sizeof (char), sz-1, file);
    fclose (file);
    if (fileLenght != nullptr)
            *fileLenght = sz;
    buffer[sz-1] = 0;
    return buffer;
}
