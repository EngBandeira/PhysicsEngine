#pragma once
#include <vector>

#include "common.hpp"

struct File {
    char *simpleName, // complete name = simpleName + '.' + extension
         *extension,
         *completeName;

    FILE_TYPES type = FILE_TYPES::COMMON_FILE;
};

struct Assets {
    std::vector<File> files;

    unsigned int framesDelay = ASSETS_DELAY,
                 delayCounter = 0;

    char *directory;
    Assets();
    ~Assets();
    void update();
};
