#include "common.hpp"
#include "log.hpp"
#include "render.hpp"
#include "utils.hpp"

Log logger = Log();
Utils utils = Utils();

glm::vec3 axis[3] = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f }
};
