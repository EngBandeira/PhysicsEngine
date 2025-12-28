#include "texture.hpp"
#include "vendor/glad/glad.h"
#include "utils.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <glm/ext/vector_float3.hpp>

template<typename T, size_t N>
bool matchPairs(T *buffer, int j, const T (&pairs)[N]) {
    for(size_t i = 0; i < N; i++) {
        if(buffer[i+j] != pairs[i]) return false;
    }
    return true;
}


// MaterialGenData *mtl(const char *path,unsigned int *materialsCount) {
//     unsigned int fileLenght;
//     MaterialGenData *contex = (MaterialGenData*)malloc(0);
//     unsigned int contexCount = 0;
//     char *buffer = readFile(path, &fileLenght);
//     char c;
//     unsigned int i = 0;
//     while(1) {
//         //Always on the line init
//         c = buffer[i];

//         if(c == '\n'){
//             i++;
//             continue;
//         }

//         if(c == '#'){
//             while(buffer[i] != '\n')
//                 i++;
//             i++;
//             continue;
//         }
//         if( matchPairs(buffer, i, {'n', 'e', 'w', 'm', 't', 'l', ' '}) ){
//             i += 7;
//             int k = i;
//             while(buffer[i] != '\n' && buffer[i] != ' ')
//                 i++;

//             contexCount++;
//             contex = (MaterialGenData*)realloc(contex,contexCount*sizeof(MaterialGenData));
//             contex[contexCount-1].name = (char*)malloc(i-k+1);
//             memcpy(contex[contexCount-1].name,buffer + k,i-k);
//             contex[contexCount-1].name[i-k] = 0;
//             i++;
//             continue;
//         }
//         if(c == 'N' || c == 'd'){
//             if(buffer[i+1] == 'i')
//                 i++;
//             if(buffer[i+1] == ' '){
//                 i += 2;
//                 unsigned int k = i;
//                 while(buffer[i] != '\n' && buffer[i] != ' ')
//                     i++;
//                 char *localBuff = (char*)malloc(i-k+1);
//                 memcpy(localBuff,buffer + k,i-k);
//                 localBuff[i-k] = 0;
//                 float f = atof(localBuff);
//                 free(localBuff);
//                 if(buffer[k-2] == 'd'){
//                     contex[contexCount -1].d = f;
//                 }
//                 else{
//                     contex[contexCount -1].Ni = f;
//                 }

//                 i++;
//                 continue;
//             }
//         }
//         if( matchPairs(buffer, i, { 'K', 'a', ' '}) || matchPairs(buffer, i, { 'K', 's', ' '}) || matchPairs(buffer, i, { 'K', 'd', ' '}) ) {
//             unsigned int vecIndex;
//             switch (buffer[i+1]) {
//                 case 'a':{
//                     vecIndex = 0;
//                     break;
//                 }
//                 case 'd':{
//                     vecIndex = 1;
//                     break;
//                 }
//                 case 's':{
//                     vecIndex = 2;
//                     break;
//                 }
//             }

//             i += 3;
//             for(unsigned int j = 0; j < 3; j++){
//                 unsigned int k = i;
//                 while(buffer[i] != '\n' && buffer[i] != ' ')
//                     i++;
//                 char *localBuff = (char*)malloc(i-k+1);
//                 memcpy(localBuff,buffer + k,i-k);
//                 localBuff[i-k] = 0;
//                 contex[contexCount-1].K[vecIndex][j] = atof(localBuff);
//                 free(localBuff);
//                 i++;
//             }
//             continue;
//         }

//         if( matchPairs(buffer, i, {'m', 'a', 'p', '_'}) ) {
//             i += 4;
//             unsigned int mapIndex = 0;
//             if(buffer[i] == 'K' && buffer[i+1] == 'd' && buffer[i+2] == ' ')
//                 mapIndex = 0;
//             else if(buffer[i] == 'N' && buffer[i+1] == 's' && buffer[i+2] == ' ')
//                 mapIndex = 1;
//             else if(buffer[i] == 'r' && buffer[i+1] == 'e' && buffer[i+2] == 'f' && buffer[i+3] == 'l' && buffer[i+4] == ' ')
//                 mapIndex = 2;
//             else if(buffer[i] == 'B' && buffer[i+1] == 'u' && buffer[i+2] == 'm' && buffer[i+3] == 'p' && buffer[i+4] == ' ')
//                 mapIndex = 3;

//             i += 5;
//             if(mapIndex == 3){
//                 if(buffer[i] == '-' && buffer[i+1] == 'b' && buffer[i+2] == 'm' && buffer[i+3] == ' '){
//                     i += 4;
//                     unsigned int k = i;
//                     while(buffer[i] != '\n' && buffer[i] != ' ')
//                         i++;
//                     char *localBuff = (char*)malloc(i-k+1);
//                     memcpy(localBuff,buffer + k,i-k);
//                     localBuff[i-k] = 0;
//                     contex[contexCount-1].bm = atof(localBuff);
//                     free(localBuff);
//                     i++;
//                 }
//             }
//             unsigned int barI = i-1;
//             while(buffer[i] != '\n' && buffer[i] != ' '){
//                 if(buffer[i] == '/')
//                     barI = i;
//                 i++;
//             }
//             contex[contexCount-1].maps[mapIndex] = (char*)malloc(i-barI);
//             memcpy(contex[contexCount-1].maps[mapIndex],buffer + barI +1,i-barI-1);
//             contex[contexCount-1].maps[mapIndex][i-barI-1] = 0;
//             continue;
//         }

//         if( matchPairs(buffer, i, {'i', 'l', 'l', 'u', 'm', ' '}) ) {
//             i += 6;
//             int k = i;
//             while(buffer[i] != '\n' && buffer[i] != ' ')
//                 i++;
//             char *localBuff = (char*)malloc(i-k+1);
//             memcpy(localBuff,buffer + k,i-k);
//             localBuff[i-k] = 0;
//             contex[contexCount-1].illum = atoi(localBuff);
//             free(localBuff);
//             continue;
//         }
//         if(!c)
//             break;
//         i++;
//     }
//     *materialsCount = contexCount;
//     free(buffer);
//     return contex;
// }

// // Texture::Texture(const char *path):localPath (path), width (0), height (0), BPP (0),position(0, 0){}
