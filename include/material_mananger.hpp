#pragma once

#include "material.hpp"
class Material_Manager {
    unsigned int material_alredy_exist(MaterialGenData gen_data);

    public:
    Material *materials;
    unsigned int materials_count = 0;

    unsigned int get_material(MaterialGenData gen_data, bool *existed);

    void init();
    void free_data();

    Material_Manager() {}
};
