#include "render/camera.hpp"
#include "common.hpp"
#include "utils.hpp"
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

void Camera::updateRotation(glm::vec3 rotation) {
    this->rotation = utils.getAngleMatrix(rotation);
    updateMatrix();
}
void Camera::updatePosition(glm::vec3 position) {
    this->translation = utils.getPositionMatrix(-position);
    updateMatrix();
}

void Camera::updateRotation(glm::mat4 rotation) {
    this->rotation = rotation;
    updateMatrix();
}

void Camera::updatePosition(glm::mat4 position) {
    this->translation = position;
    updateMatrix();
}

void Camera::updateMatrix(glm::mat4 position, glm::mat4 rotation) {
    this->translation = position;
    this->rotation = rotation;
    updateMatrix();
}
void Camera::updateMatrix(glm::vec3 position, glm::vec3 rotation) {
    this->translation = utils.getPositionMatrix(-position);
    this->rotation = utils.getAngleMatrix(rotation);
    updateMatrix();
}

void Camera::updateMatrix(glm::vec3 position, glm::mat4 rotation) {
    this->translation = utils.getPositionMatrix(-position);
    this->rotation = rotation;
    updateMatrix();
}



void Camera::updateMatrix() {
    viewMatrix = rotation * translation;
}

Camera::Camera() :  rotation(1), translation(1)
{ projMatrix = glm::perspective(glm::radians(45.0f), (float)SCR_X / (float)SCR_Y, 0.1f, 100.0f); }
