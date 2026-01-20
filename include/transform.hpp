#pragma once
#include "glm/glm.hpp"
class Transform {
  public:
    void scale(glm::vec3 scale, unsigned short index,unsigned int layerIndex);
    void scale(float scale, unsigned short index,unsigned int layerIndex);
    void translate(glm::vec3 translation, unsigned short index,unsigned int layerIndex);
    void rotate(glm::vec3 rotation, unsigned short index,unsigned int layerIndex);
    void set_angle(glm::vec3 angle, unsigned short index,unsigned int layerIndex);
    void positionate(glm::vec3 position, unsigned short index,unsigned int layerIndex);

    glm::vec3 axis[3];
    glm::mat4 get_scale_matrix(glm::vec3 scale);
    glm::mat4 get_angle_matrix(glm::vec3 angle_radians);
    glm::mat4 get_position_matrix(glm::vec3 position);
    glm::mat4 aim_matrix(glm::vec3 direction);

    glm::mat4 *get_matrix(unsigned short index,unsigned int layerIndex);
    glm::vec4 get_position(unsigned short index,unsigned int layerIndex);
    // Transform();
};
