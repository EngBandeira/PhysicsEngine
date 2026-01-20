#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "common.hpp"

// void Camera::updateMatrix(glm::mat4 position, glm::mat4 rotation) {
//     this->translation = position;
//     this->rotation = rotation;
//     updateMatrix();
// }

// void Camera::updateMatrix(glm::vec3 position, glm::vec3 rotation) {
//     this->translation = (-position);
//     this->rotation = utils.getAngleMatrix(rotation);
//     updateMatrix();
// }

// void Camera::updateMatrix(glm::vec3 position, glm::mat4 rotation) {
//     this->translation = utils.getPositionMatrix(-position);
//     this->rotation = rotation;
//     updateMatrix();
// }

// void Camera::updateMatrix() {
//     viewMatrix = rotation * translation;
// }
