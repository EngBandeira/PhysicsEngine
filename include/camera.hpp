#pragma once
#include "glm/glm.hpp"

class Camera {
    public:
    glm::vec4 *up,*foward,*right;
    glm::vec3 angle;
    glm::mat4 rotation, translation, viewMatrix, projMatrix;
    glm::vec4 getPosition();
    glm::vec4 getUp();
    glm::vec4 getFoward();
    glm::vec4 getRight();
    // void updateRotation(glm::vec3 rotation);
    // void updatePosition(glm::vec3 position);

    // void updateRotation(glm::mat4 rotation);
    // void updatePosition(glm::mat4 position);

    // void updateMatrix(glm::mat4 position, glm::mat4 rotation);
    // void updateMatrix(glm::vec3 position, glm::vec3 rotation);
    // void updateMatrix(glm::vec3 position, glm::mat4 rotation);
    // void updateMatrix();
    Camera();
};
