#pragma once

#include "material.hpp"
#include "common.hpp"

void setTexParameter(unsigned int tex_type);


struct Texture_Handler {
    unsigned int texture,
                 texDimensions,
                  emptyTexturesCount,
                  texturesCount = 0,

                  // Pointers
                  *materialIndex,
                  *emptyTextures;

    unsigned int add_tex(unsigned char *localBuffer);
    void rm_tex(unsigned int index);
    Texture_Handler() = default;
};

struct Vadia_Texture {
    char* name;
    unsigned int code;
    TextureLocation location;
};


class Texture_Mananger {
    TextureLocation add_tex_to_handler(char *local_path, bool to_process = 1);
    unsigned int texture_alredy_exist(unsigned int code);

    public:
    Texture_Handler textureHandlers[TEXTURE_HANDLERS_COUNT];
    Vadia_Texture *textures;
    unsigned int textures_count = 0;



    unsigned int get_texture(char* path);

    void init();
    void free_data();

    Texture_Mananger() {}
};
