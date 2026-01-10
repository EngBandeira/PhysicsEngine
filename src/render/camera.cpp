#include "render/camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec4 Camera::getUp(){
    return translation * glm::vec4(rotation[0][1],rotation[1][1],rotation[2][1],rotation[3][1]);
}

glm::vec4 Camera::getFoward(){
    return translation * glm::vec4(rotation[0][2],rotation[1][2],rotation[2][2],rotation[3][2]) * -1.f;
}

glm::vec4 Camera::getRight(){
    return translation * glm::vec4(rotation[0][0],rotation[1][0],rotation[2][0],rotation[3][0]);
}

glm::vec4 Camera::getPosition(){
    return translation * glm::vec4(0,0,0,1) * (-1.0f);
}

Camera::Camera() : translation(1), rotation(1), localTranslation(1)
{

    projMatrix = glm::perspective(glm::radians(45.0f),
                                    (float)SCR_X / (float)SCR_Y, 0.1f, 100.0f);
    viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
