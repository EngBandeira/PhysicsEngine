#pragma once
#include "glm/glm.hpp"

class Transform {
  public:
    glm::mat4 translation_matrix, rotation_matrix, scale_matrix, matrix;
    glm::vec3 angle;
    void scale(glm::vec3 scale);
    void scale(float scale);
    void translate(glm::vec3 translation);
    void rotate(glm::vec3 rotation);
    void set_angle(glm::vec3 angle);
    void positionate(glm::vec3 position);
    void updateMatrix();

    // glm::mat4 get_scale_matrix(glm::vec3 scale);
    // glm::mat4 get_angle_matrix(glm::vec3 angle_radians);
    // glm::mat4 get_position_matrix(glm::vec3 position);
    // glm::mat4 aim_matrix(glm::vec3 direction);


    glm::vec3 get_position();
    glm::vec3 get_rotation();
    glm::vec3 get_scale();
    Transform(): translation_matrix(1), rotation_matrix(1), scale_matrix(1), matrix(1), angle(0) {};
};
