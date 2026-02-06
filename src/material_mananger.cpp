#include "material_mananger.hpp"
#include "common.hpp"
#include "material.hpp"
#include "engine.hpp"
#include <cstdlib>
#include <cstring>




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
        if(m.K[0] != gen_data.K[0])    continue;
        if(m.K[1] != gen_data.K[1])    continue;
        if(m.K[2] != gen_data.K[2])    continue;
        if(m.Ni != gen_data.Ni) continue;
        if(m.d != gen_data.d)   continue;
        if(m.bm != gen_data.bm) continue;
        if(m.type != gen_data.type) continue;

        if(gen_data.maps[0] != 0)
           if(m.maps[0] > engine.texture_manager.textures_count || codes[0] != engine.texture_manager.textures[m.maps[0]].code)
               continue;
        if(gen_data.maps[1] != 0)
           if(m.maps[1] > engine.texture_manager.textures_count || codes[1] != engine.texture_manager.textures[m.maps[1]].code)
               continue;
        if(gen_data.maps[2] != 0)
           if(m.maps[2] > engine.texture_manager.textures_count || codes[2] != engine.texture_manager.textures[m.maps[2]].code)
               continue;
        if(gen_data.maps[3] != 0)
           if(m.maps[3] > engine.texture_manager.textures_count || codes[3] != engine.texture_manager.textures[m.maps[3]].code)
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
    if(gen_data.maps[0] == nullptr)
        material.type = Material_Type::SOLID_COLOR;

    material.K[0] = gen_data.K[0];
    material.K[1] = gen_data.K[1];
    material.K[2] = gen_data.K[2];

    for( int i = 0; i < 4; i++ ) {
        if( gen_data.type == TEXTURE && gen_data.maps[i] != nullptr ) material.maps[i] = engine.texture_manager.get_texture(gen_data.maps[i]);
        else material.maps[i] = -1;
    }

    MaterialCopy material_copy;

    memcpy(&material_copy, &material, 64);
    if(material.maps[0] != (unsigned int)-1)
        material_copy.maps[0] = engine.texture_manager.textures[material.maps[0]].location;
    if(material.maps[1] != (unsigned int)-1)
        material_copy.maps[1] = engine.texture_manager.textures[material.maps[1]].location;
    if(material.maps[2] != (unsigned int)-1)
        material_copy.maps[2] = engine.texture_manager.textures[material.maps[2]].location;
    if(material.maps[3] != (unsigned int)-1)
        material_copy.maps[3] = engine.texture_manager.textures[material.maps[3]].location;

    unsigned int &materials_ssbo = engine.render_data.ssbos[COMMON_BY_SSBOS[SSBOS::MaterialsSSBO]];

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, engine.render_data.buffer_utilitary);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(MaterialCopy) * materials_count, 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_COPY_READ_BUFFER, materials_ssbo);
    glBindBuffer(GL_COPY_WRITE_BUFFER, engine.render_data.buffer_utilitary);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
            sizeof(MaterialCopy) * (materials_count - 1));


    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);


    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
            sizeof(MaterialCopy) * (materials_count - 1),
            sizeof(MaterialCopy), &material_copy);

    unsigned int temp = engine.render_data.buffer_utilitary;
    engine.render_data.buffer_utilitary = materials_ssbo;
    materials_ssbo = temp;


    glBindBuffer(GL_SHADER_STORAGE_BUFFER, engine.render_data.buffer_utilitary);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

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
