#include "transform.hpp"
#include "common.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <bits/stdc++.h>

#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void Transform::scale(glm::vec3 scale) {
    scale_matrix[0][0] = scale.x;
    scale_matrix[1][1] = scale.y;
    scale_matrix[2][2] = scale.z;
    updateMatrix();
}

void Transform::scale(float scale) {
    this->scale(glm::vec3(scale));
}

void Transform::positionate(glm::vec3 position) {
    translation_matrix[3][0] = position.x;
    translation_matrix[3][1] = position.y;
    translation_matrix[3][2] = position.z;

    updateMatrix();
}


void Transform::translate(glm::vec3 translation) {
    translation_matrix[3][0] += translation.x;
    translation_matrix[3][1] += translation.y;
    translation_matrix[3][2] += translation.z;

    updateMatrix();
}

void Transform::set_angle(glm::vec3 angle) {

    if(this->angle.x == angle.x && this->angle.y == angle.y && this->angle.z == angle.z) {
        return;
    }
    rotation_matrix = glm::rotate(glm::mat4(1), angle.x, axis[0]);
    rotation_matrix = glm::rotate(rotation_matrix, angle.y, axis[1]);
    rotation_matrix = glm::rotate(rotation_matrix, angle.z, axis[2]);
    this->angle = angle;

    updateMatrix();
}

void Transform::rotate(glm::vec3 rotation) {
    if(rotation[0] == 0 && rotation[1] == 0  && rotation[2] == 0 ) {
        return;
    }

    angle += rotation;
    rotation_matrix = glm::rotate(glm::mat4(1), angle.x, axis[0]);
    rotation_matrix = glm::rotate(rotation_matrix, angle.y, axis[1]);
    rotation_matrix = glm::rotate(rotation_matrix, angle.z, axis[2]);

    updateMatrix();
}

glm::vec3 Transform::get_position() {
    return (translation_matrix * glm::vec4(0, 0, 0, 1));
}
glm::vec3 Transform::get_scale() {
    return glm::vec3(scale_matrix[0][0],scale_matrix[1][1],scale_matrix[2][2]);
}


glm::vec3 Transform::get_rotation() {
    return angle;
}


void Transform::updateMatrix() {
    matrix = translation_matrix * rotation_matrix * scale_matrix;
    flags = flags | MATRICES_CHANGE_FLAG;
}
