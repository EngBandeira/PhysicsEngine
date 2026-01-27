#pragma once
#include <vector>

#include "common.hpp"

struct File {
    char *simpleName, // complete name = simpleName + '.' + extension
         *extension,
         *completeName;
    ~File();
    FILE_TYPES type = FILE_TYPES::COMMON_FILE;
};

struct Assets {
    File *files;

    unsigned int files_number = 1,
                 framesDelay = ASSETS_DELAY,
                 delayCounter = 0;

    char *directory;
    void init();
    void free_data();
    void update();
};
