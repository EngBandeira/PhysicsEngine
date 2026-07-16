#include "utils/common.hpp"
#include "log/log.hpp"
#include "utils.hpp"
#include "butia.hpp"

Log logger = Log();
Utils utils = Utils();
Butia *butia;
glm::vec3 axis[3] = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f }
};
