#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


glm::mat4 Camera::get_view(){
    return transform.rotation_matrix * transform.translation_matrix;
}
