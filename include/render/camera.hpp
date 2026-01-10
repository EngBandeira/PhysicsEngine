#pragma once
#include "glm/glm.hpp"
#include "common.hpp"

class Camera {
    public:
    glm::vec4 *up,*foward,*right;
    glm::vec3 angle;
    glm::mat4 translation, rotation, localTranslation, viewMatrix, projMatrix;
    glm::vec4 getPosition();
    glm::vec4 getUp();
    glm::vec4 getFoward();
    glm::vec4 getRight();
    Camera();
};
