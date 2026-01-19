#pragma once
#include <glm/glm.hpp>

class Utils {
public:
    char *readFile(const char *localPath, unsigned int *fileLenght, const char *flags = "r");

    glm::vec3 axis[3];

    glm::mat4 getScaleMatrix(glm::vec3 scale);

    glm::mat4 getAngleMatrix(glm::vec3 angle_radians);

    glm::mat4 getPositionMatrix(glm::vec3 position);

    glm::mat4 aimMatrix(glm::vec3 direction);


    Utils();

    template<typename T, size_t N>
    bool matchPairs(T *buffer, int j, const T (&pairs)[N]) {
        for(size_t i = 0; i < N; i++) {
            if(buffer[i+j] != pairs[i]) return false;
        }
        return true;
    }
};
