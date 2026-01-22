#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 Camera::get_up() {
    return -transform.translation_matrix * glm::vec4(transform.rotation_matrix[0][1],transform.rotation_matrix[1][1],transform.rotation_matrix[2][1],transform.rotation_matrix[3][1]);
}

glm::vec3 Camera::get_foward() {
    return  transform.translation_matrix * glm::vec4(transform.rotation_matrix[0][2],transform.rotation_matrix[1][2],transform.rotation_matrix[2][2],transform.rotation_matrix[3][2]);
}

glm::vec3 Camera::get_right() {
    return -transform.translation_matrix * glm::vec4(transform.rotation_matrix[0][0],transform.rotation_matrix[1][0],transform.rotation_matrix[2][0],transform.rotation_matrix[3][0]);
}

glm::vec3 Camera::get_position() {
    return (transform.translation_matrix * glm::vec4(0,0,0,1));
}
