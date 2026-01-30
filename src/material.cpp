#include "material.hpp"
#include "render.hpp"

MaterialGenData::MaterialGenData(char *map_zero) {
    maps[0] = map_zero;
    maps[1] = nullptr;
    maps[2] = nullptr;
    maps[3] = nullptr;
}
MaterialGenData::MaterialGenData() {
    K[0] = glm::vec3(1,0,0);
    maps[0] = nullptr;
    maps[1] = nullptr;
    maps[2] = nullptr;
    maps[3] = nullptr;
}
