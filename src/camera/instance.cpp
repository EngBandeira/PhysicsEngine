#include "camera.hpp"
#include "common.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera() : rotation(1), translation(1) {
    projMatrix = glm::perspective(glm::radians(45.0f), (float)SCR_X / (float)SCR_Y, 0.1f, 100.0f);
}
