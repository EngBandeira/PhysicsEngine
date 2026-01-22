#pragma once
#include "glm/glm.hpp"
#include "transform.hpp"

class Camera {
    public:
    Transform transform;
    glm::mat4 proj_matrix;
    glm::vec3 get_position();
    glm::vec3 get_up();
    glm::vec3 get_foward();
    glm::vec3 get_right();
    glm::mat4 get_view();
    Camera();
};
