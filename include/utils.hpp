#pragma once
#include <glm/glm.hpp>

class Utils {
public:
    char *readFile(const char *localPath, unsigned int *fileLenght, const char *flags = "r");

    template<typename T, size_t N>
    bool matchPairs(T *buffer, int j, const T (&pairs)[N]) {
        for(size_t i = 0; i < N; i++) {
            if(buffer[i+j] != pairs[i]) return false;
        }
        return true;
    }
};
