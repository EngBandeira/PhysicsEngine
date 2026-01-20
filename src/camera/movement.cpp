#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec4 Camera::getUp() {
    return translation * glm::vec4(rotation[0][1],rotation[1][1],rotation[2][1],rotation[3][1]);
}

glm::vec4 Camera::getFoward() {
    return translation * glm::vec4(rotation[0][2],rotation[1][2],rotation[2][2],rotation[3][2]) * -1.f;
}

glm::vec4 Camera::getRight() {
    return translation * glm::vec4(rotation[0][0],rotation[1][0],rotation[2][0],rotation[3][0]);
}

glm::vec4 Camera::getPosition() {
    return translation * glm::vec4(0,0,0,1) * -1.f;
}
