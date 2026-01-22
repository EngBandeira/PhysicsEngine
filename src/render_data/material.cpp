#include "common.hpp"
#include "material.hpp"
#include "log.hpp"
#include "render_data.hpp"
#include <cstdlib>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>

#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>


unsigned int RenderData::allocMaterial(MaterialGenData data) {
    materialsCount++;
    materials = (Material*) realloc(materials, sizeof(Material) * materialsCount);
    Material *mater = materials + materialsCount - 1;

    mater->type = data.type;
    mater->bm   = data.bm;
    mater->Ni   = data.Ni;
    mater->d    = data.d;

    for( int i = 0; i < 3; i++ ) {
        mater->K[3 * i]     = data.K[i].x;
        mater->K[3 * i + 1] = data.K[i].y;
        mater->K[3 * i + 2] = data.K[i].z;
    }

    for( int i = 0; i < 4; i++ ) {
        if( data.type == TEXTURE && data.maps[i] != nullptr ) mater->maps[i] = addTexToHandler(data.maps[i]);
        else mater->maps[i] = TextureLocation {.handler = -1};
    }

    flags = flags | MATERIAL_CHANGE_FLAG;
    return materialsCount - 1;
}

void RenderData::freeMaterial(unsigned int index) {
    Material *newMat = (Material*) malloc(sizeof(Material) * (materialsCount - 1));

    if( index < 0 ) {
        logger.sendError("freeMaterial: index < 0", 0);
        return;
    }

    if( index >= materialsCount ) {
        logger.sendError("freeMaterial: index >= materialsCount", 0);
        return;
    }

    memcpy(newMat, materials, index);
    memcpy(newMat + index + 1, materials + index + 1, materialsCount - 1 - index);

    free(materials);

    materials = newMat;
    materialsCount--;

    flags = flags | MATERIAL_CHANGE_FLAG;
}
