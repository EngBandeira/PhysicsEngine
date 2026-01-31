#include "texture_manager.hpp"
#include "common.hpp"
#include "render.hpp"
#include "vendor/stb_image/stb_image.h"
#include <cstdlib>


TextureLocation Texture_Mananger::add_tex_to_handler(char *local_path, bool to_process) {
    char *path = local_path;
    if( to_process ) {
        int i = 0, k;

        while( path[i] != 0 ) {
            if( path[i] == '.' ) k = i;
            i++;
        }

        char *newPath = (char*) malloc(k + RAW_TEX_EXTENSION_LENGHT + 2); //path + '.' + RAW_TEX_EXTENSION + '\0'
        memcpy(newPath, path, k+1);
        for( int j = 0; j < RAW_TEX_EXTENSION_LENGHT; j++ ) {
            newPath[k + 1 + j] = RAW_TEX_EXTENSION[j];
        }

        newPath[k + 1 + RAW_TEX_EXTENSION_LENGHT] = 0;
        if( access(newPath, F_OK) ) {
            int width, height, bpp;
            stbi_set_flip_vertically_on_load (1);
            //localBuffeSize = 4 * width * width
            unsigned char *localBuffer = stbi_load(path, &width,  &height, &bpp, 4);

            if(local_path == nullptr) logger.sendError("image do not exist", 1);
            if(width != height) logger.sendError("Width != Height", 1);
            if(width > 4096) logger.sendError("Width, Height > 4096", 1);


            path = newPath;
            FILE *file = fopen(path, "wb");
            char c[14]; // 13 is the lenght of 0xFFRAWIMGWWHHBB
            sprintf(c, "\xffRAWIMG");
            fwrite(c, sizeof(char), 7, file);
            char f[6];
            f[0] = (unsigned char)width;
            f[1] = (unsigned char)(width >> 8);
            f[2] = (unsigned char)height;
            f[3] = (unsigned char)(height >> 8);
            f[4] = (unsigned char)bpp;
            f[5] = (unsigned char)(bpp >> 8);
            fwrite(f, sizeof(char), 6, file);

            fwrite(localBuffer, 1, 4 * width * width, file);
            fclose(file);
        }
        else {
            path = newPath;
            // free(newPath);
        }
    }


    unsigned int lenght;
    unsigned char* localBuffer = (unsigned char*)utils.read_file(path, &lenght,"rb");
    if(! utils.match_pairs(localBuffer, 0, (unsigned char[]){(unsigned char)'\xff', 'R', 'A', 'W', 'I', 'M', 'G'}) ) {
        logger.sendError("addTexToHandler, file of wrong type");
        return TextureLocation();
    }

    unsigned short width = *((unsigned short*)(localBuffer + 7));

    int handlerIndex = (unsigned char)floor(log2(width)-6);
    unsigned int index = textureHandlers[handlerIndex].add_tex((unsigned char*)localBuffer+13);
    free(localBuffer);

    if( to_process ) free(path);

    return TextureLocation {
        .handler = handlerIndex,
        .index = index
    };
}

unsigned int Texture_Handler::add_tex(unsigned char *localBuffer){
    if(emptyTexturesCount) {
        unsigned int i = emptyTextures[0];
        glBindTexture(GL_TEXTURE_2D_ARRAY,texture);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, LEVEL, 0, 0,
                            i-1, texDimensions, texDimensions, 1, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);

        emptyTexturesCount--;
        emptyTextures = (unsigned int*)realloc(emptyTextures, emptyTexturesCount * sizeof(int));
        return i;
    }


    glBindTexture(GL_TEXTURE_2D_ARRAY, render.render_data.texUtilitary);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,texDimensions,texDimensions,texturesCount+1,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);

    setTexParameter(GL_TEXTURE_2D_ARRAY);


    glCopyImageSubData(texture,GL_TEXTURE_2D_ARRAY,LEVEL,0,0,0,render.render_data.texUtilitary, GL_TEXTURE_2D_ARRAY, LEVEL,
                    0,0,0,texDimensions,texDimensions,texturesCount);

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, LEVEL, 0, 0,
                        texturesCount, texDimensions, texDimensions, 1, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);

    glBindTexture(GL_TEXTURE_2D_ARRAY,texture);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 , 0, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D_ARRAY,0);

    texturesCount++;

    unsigned int cache = texture;
    texture = render.render_data.texUtilitary;
    render.render_data.texUtilitary = cache;

    return texturesCount-1;
}

// void Texture_Handler::rm_tex(unsigned int index) {
//     emptyTexturesCount++;

//     if( emptyTexturesCount > MAX_EMPTY_TEXTURES_COUNT
//         || (emptyTexturesCount > MIN_EMPTY_TEXTURES_COUNT_TO_RST && emptyTexturesCount > texturesCount * MAX_RATIO_OF_EMPTY_TEXTURES)
//     ) return;

//     emptyTextures = (unsigned int*) realloc(emptyTextures, emptyTexturesCount);
//     emptyTextures[emptyTexturesCount-1] = index;
// }

unsigned int Texture_Mananger::texture_alredy_exist(unsigned int code) {
    for(unsigned int i = 0; i < textures_count; i++) {
        if(textures[i].code == code)
            return i;
    }
    return -1;
}

unsigned int Texture_Mananger::get_texture(char* path) {
    unsigned int code = utils.sha256(path);

    unsigned int i = texture_alredy_exist(code);
    if(i != (unsigned int)-1)
        return i;

    textures = (Vadia_Texture*)realloc(textures, sizeof(Vadia_Texture) * ++textures_count);
    textures[textures_count - 1].location = add_tex_to_handler(path);
    textures[textures_count - 1].name = path;
    textures[textures_count - 1].code = code;
    return textures_count - 1;
}
void Texture_Mananger::init() {
    textures = (Vadia_Texture*)malloc(0);
    for( unsigned int i = 0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        textureHandlers[i].materialIndex = (unsigned int *) malloc(0);
        textureHandlers[i].texDimensions = pow(2, i + 6);
        textureHandlers[i].emptyTextures = (unsigned int *) malloc(0);
        textureHandlers[i].emptyTexturesCount = 0;
        textureHandlers[i].texturesCount = 0;

        glGenTextures(1,&textureHandlers[i].texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY,textureHandlers[i].texture);
        glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 , textureHandlers[i].texDimensions,
            textureHandlers[i].texDimensions,0,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);

        setTexParameter(GL_TEXTURE_2D_ARRAY);
    }
}
void Texture_Mananger::free_data() {
    free(textures);

    for( unsigned int i = 0; i < TEXTURE_HANDLERS_COUNT; i++ ) {
        free(textureHandlers[i].materialIndex);
        free(textureHandlers[i].emptyTextures);
    }
}
