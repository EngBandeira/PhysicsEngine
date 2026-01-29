#include "material.hpp"

MaterialGenData::MaterialGenData(char *map_zero) {
    maps[0] = map_zero;
    maps[1] = nullptr;
    maps[2] = nullptr;
    maps[3] = nullptr;
}
MaterialGenData::MaterialGenData() {
    maps[0] = nullptr;
    maps[1] = nullptr;
    maps[2] = nullptr;
    maps[3] = nullptr;
}
