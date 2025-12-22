#include "texture.hpp"
#include "glad/glad.h"
#include "utils.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <glm/ext/vector_float3.hpp>

// # Blender 4.5.3 LTS MTL File: 'cannon_01_4k.blend'
// # www.blender.org

// newmtl cannon_01
// Ka 1.000000 1.000000 1.000000
// Ks 0.500000 0.500000 0.500000
// Ke 0.000000 0.000000 0.000000
// Ni 1.450000
// d 1.000000
// illum 2
// map_Kd /home/bandeira/Downloads/cannon_01_4k.blend/textures/cannon_01_diff_4k.jpg
// map_Ns /home/bandeira/Downloads/cannon_01_4k.blend/textures/cannon_01_rough_4k.exr
// map_refl /home/bandeira/Downloads/cannon_01_4k.blend/textures/cannon_01_metal_4k.exr
// map_Bump -bm 1.000000 /home/bandeira/Downloads/cannon_01_4k.blend/textures/cannon_01_nor_gl_4k.exr



void Material::deleteMaterial(){
    free(name);
    free(maps[0]);
    free(maps[1]);
    free(maps[2]);
    free(maps[3]);
}

Material *mtl(const char *path,unsigned int *materialsCount)
{
    unsigned int fileLenght;
    Material *contex = (Material*)malloc(0);
    unsigned int contexCount = 0;
    char *buffer = readFile(path, &fileLenght);
    char c;
    unsigned int i = 0;
    while (1) {
        //Always on the line init
        c = buffer[i];

        if(c == '\n'){
            i++;
            continue;
        }

        if(c == '#'){
            while(buffer[i] != '\n')
                i++;
            i++;
            continue;
        }
        if(c == 'n' && buffer[i+1] == 'e' && buffer[i+2] == 'w' && buffer[i+3] == 'm' &&
            buffer[i+4] == 't' && buffer[i+5] == 'l' && buffer[i+6] == ' '){
            i += 7;
            int k = i;
            while(buffer[i] != '\n' && buffer[i] != ' ')
                i++;

            contexCount++;
            contex = (Material*)realloc(contex,contexCount*sizeof(Material));
            contex[contexCount-1].name = (char*)malloc(i-k+1);
            memcpy(contex[contexCount-1].name,buffer + k,i-k);
            contex[contexCount-1].name[i-k] = 0;
            i++;
            continue;
        }
        if(c == 'N' || c == 'd'){
            if(buffer[i+1] == 'i')
                i++;
            if(buffer[i+1] == ' '){
                i += 2;
                unsigned int k = i;
                while(buffer[i] != '\n' && buffer[i] != ' ')
                    i++;
                char *localBuff = (char*)malloc(i-k+1);
                memcpy(localBuff,buffer + k,i-k);
                localBuff[i-k] = 0;
                float f = atof(localBuff);
                free(localBuff);
                if(buffer[k-2] == 'd'){
                    contex[contexCount -1].d = f;
                }
                else{
                    contex[contexCount -1].Ni = f;
                }

                i++;
                continue;
            }
        }
        if(c == 'K' && (buffer[i+1] == 'a' || buffer[i+1] == 's' || buffer[i+1] == 'e') && buffer[i+2] == ' '){
            unsigned int vecIndex;
            switch (buffer[i+1]) {
                case 'a':{
                    vecIndex = 0;
                    break;
                }
                case 's':{
                    vecIndex = 1;
                    break;
                }
                case 'e':{
                    vecIndex = 2;
                    break;
                }
            }

            i += 3;
            for(unsigned int j = 0; j < 3; j++){
                unsigned int k = i;
                while(buffer[i] != '\n' && buffer[i] != ' ')
                    i++;
                char *localBuff = (char*)malloc(i-k+1);
                memcpy(localBuff,buffer + k,i-k);
                localBuff[i-k] = 0;
                contex[contexCount-1].K[vecIndex][j] = atof(localBuff);
                free(localBuff);
                i++;
            }
            continue;
        }
        if(c == 'm' && buffer[i+1] == 'a' && buffer[i+2] == 'p' && buffer[i+3] == '_'){
            i += 4;
            unsigned int mapIndex = 0;
            if(buffer[i] == 'K' && buffer[i+1] == 'd' && buffer[i+2] == ' ')
                mapIndex = 0;
            else if(buffer[i] == 'N' && buffer[i+1] == 's' && buffer[i+2] == ' ')
                mapIndex = 1;
            else if(buffer[i] == 'r' && buffer[i+1] == 'e' && buffer[i+2] == 'f' && buffer[i+3] == 'l' && buffer[i+4] == ' ')
                mapIndex = 2;
            else if(buffer[i] == 'B' && buffer[i+1] == 'u' && buffer[i+2] == 'm' && buffer[i+3] == 'p' && buffer[i+4] == ' ')
                mapIndex = 3;

            i += 5;
            if(mapIndex == 3){
                if(buffer[i] == '-' && buffer[i+1] == 'b' && buffer[i+2] == 'm' && buffer[i+3] == ' '){
                    i += 4;
                    unsigned int k = i;
                    while(buffer[i] != '\n' && buffer[i] != ' ')
                        i++;
                    char *localBuff = (char*)malloc(i-k+1);
                    memcpy(localBuff,buffer + k,i-k);
                    localBuff[i-k] = 0;
                    contex[contexCount-1].bm = atof(localBuff);
                    free(localBuff);
                    i++;
                }
            }
            unsigned int barI = i-1;
            while(buffer[i] != '\n' && buffer[i] != ' '){
                if(buffer[i] == '/')
                    barI = i;
                i++;
            }
            contex[contexCount-1].maps[mapIndex] = (char*)malloc(i-barI);
            memcpy(contex[contexCount-1].maps[mapIndex],buffer + barI +1,i-barI-1);
            contex[contexCount-1].maps[mapIndex][i-barI-1] = 0;
            continue;
        }
        if(c == 'i' && buffer[i+1] == 'l'&& buffer[i+2] == 'l'&& buffer[i+3] == 'u'&& buffer[i+4] == 'm' && buffer[i+5] == ' '){
            i += 6;
            int k = i;
            while(buffer[i] != '\n' && buffer[i] != ' ')
                i++;
            char *localBuff = (char*)malloc(i-k+1);
            memcpy(localBuff,buffer + k,i-k);
            localBuff[i-k] = 0;
            contex[contexCount-1].illum = atoi(localBuff);
            free(localBuff);
            continue;
        }
        if(!c)
            break;
        i++;
    }
    *materialsCount = contexCount;
    free(buffer);
    return contex;
}

Texture::Texture(const char *path):localPath (path), width (0), height (0), BPP (0),position(0, 0)
{

}

// Texture::Texture (const char *path)
//     : localBuffer (nullptr), localPath (path), width (0), height (0), BPP (0),renderID(0)
// {
//         glGenTextures (1, &renderID);
//         glBindTexture (GL_TEXTURE_2D, renderID);
//         stbi_set_flip_vertically_on_load (1);
//         localBuffer = stbi_load (path, &width, &height, &BPP, 4);
//         glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//         glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//         glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//         glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//         glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
//                       GL_UNSIGNED_BYTE, localBuffer);
//         glGenerateMipmap(GL_TEXTURE_2D);
//         glBindTexture (GL_TEXTURE_2D, 0);

//         stbi_image_free (localBuffer);
// }
// Texture::~Texture ()
// {
//         glDeleteTextures (1, &renderID);
// }
// void
// Texture::Bind (unsigned int slot)
// {
//         Slot = slot;
//         glActiveTexture (GL_TEXTURE0 + Slot);
//         glBindTexture (GL_TEXTURE_2D, renderID);
// }
// void
// Texture::Unbind () const
// {
//         glBindTexture (GL_TEXTURE_2D, 0);
// }
