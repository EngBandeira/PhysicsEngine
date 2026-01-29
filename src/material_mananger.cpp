#include "material_mananger.hpp"
#include "common.hpp"
#include "material.hpp"
#include "render.hpp"
#include <cstdlib>




// void Render_Data::free_material(unsigned int index) {
//     Material *newMat = (Material*) malloc(sizeof(Material) * (materialsCount - 1));

//     if( index < 0 ) {
//         logger.sendError("freeMaterial: index < 0", 0);
//         return;
//     }

//     if( index >= materialsCount ) {
//         logger.sendError("freeMaterial: index >= materialsCount", 0);
//         return;
//     }

//     memcpy(newMat, materials, index);
//     memcpy(newMat + index + 1, materials + index + 1, materialsCount - 1 - index);

//     free(materials);

//     materials = newMat;
//     materialsCount--;

//     flags = flags | MATERIAL_CHANGE_FLAG;
// }



// struct MaterialGenData {
//     glm::vec3 K[3];//Ka Kd Ks
//     float Ni, d,bm;
//     char *maps[4];// Ka Kd Ks Normal
//     enum materialType type;
//     MaterialGenData();
// };

unsigned int Material_Manager::material_alredy_exist(MaterialGenData gen_data) {
    unsigned int codes[4];
    if(gen_data.maps[0] != 0)
        codes[0] = utils.sha256(gen_data.maps[0]);
    if(gen_data.maps[1] != 0)
        codes[1] = utils.sha256(gen_data.maps[1]);
    if(gen_data.maps[2] != 0)
        codes[2] = utils.sha256(gen_data.maps[2]);
    if(gen_data.maps[3] != 0)
        codes[3] = utils.sha256(gen_data.maps[3]);

    for(unsigned int i = 0; i < materials_count; i++){
        Material m = materials[i];
        if(glm::vec3(m.K[0],m.K[1],m.K[2]) != gen_data.K[0])    continue;
        if(glm::vec3(m.K[3],m.K[4],m.K[5]) != gen_data.K[0])    continue;
        if(glm::vec3(m.K[6],m.K[7],m.K[8]) != gen_data.K[0])    continue;
        if(m.Ni != gen_data.Ni) continue;
        if(m.d != gen_data.d)   continue;
        if(m.bm != gen_data.bm) continue;
        if(m.type != gen_data.type) continue;

        if(gen_data.maps[0] != 0)
           if(m.maps[0] < render.texture_manager.textures_count && codes[0] != render.texture_manager.textures[m.maps[0]].code)
               continue;
        if(gen_data.maps[1] != 0)
           if(m.maps[1] < render.texture_manager.textures_count && codes[1] != render.texture_manager.textures[m.maps[1]].code)
               continue;
        if(gen_data.maps[2] != 0)
           if(m.maps[2] < render.texture_manager.textures_count && codes[2] != render.texture_manager.textures[m.maps[2]].code)
               continue;
        if(gen_data.maps[3] != 0)
           if(m.maps[3] < render.texture_manager.textures_count && codes[3] != render.texture_manager.textures[m.maps[3]].code)
               continue;

        return i;
    }
    return -1;
}


unsigned int Material_Manager::get_material(MaterialGenData gen_data, bool *existed) {
    unsigned int i = material_alredy_exist(gen_data);
    if(i != (unsigned int)-1){
        *existed = 1;
        return i;
    }

    materials = (Material*)realloc(materials, sizeof(Material) * ++materials_count);
    Material &material = materials[materials_count - 1];

    material.type = gen_data.type;
    material.bm   = gen_data.bm;
    material.Ni   = gen_data.Ni;
    material.d    = gen_data.d;

    for( int i = 0; i < 3; i++ ) {
        material.K[3 * i]     = gen_data.K[i].x;
        material.K[3 * i + 1] = gen_data.K[i].y;
        material.K[3 * i + 2] = gen_data.K[i].z;
    }

    for( int i = 0; i < 4; i++ ) {
        if( gen_data.type == TEXTURE && gen_data.maps[i] != nullptr ) material.maps[i] = render.texture_manager.get_texture(gen_data.maps[i]);
        else material.maps[i] = -1;
    }

    flags = flags | MATERIAL_CHANGE_FLAG;
    existed = 0;
    return materials_count - 1;
}


void Material_Manager::init() {
    materials = (Material*)malloc(0);
}


void Material_Manager::free_data() {
    free(materials);
}
