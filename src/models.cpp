#include "models.hpp"
#include <stdio.h>
#include <string.h>

#define BUFFER_LENGHT 64
#define KEYWORDS_LENGTH 8

// #define LARGE_CONSTANT 100000

// const char *keywords[KEYWORDS_LENGTH]
//     = { "v", "vn", "vt", "f", "o", "s", "mtllib", "usemtl" };
// unsigned int keywordsID[KEYWORDS_LENGTH];

// unsigned int
// getId (const char *str)
// {
//     unsigned int rt = 0;
//     for (__uint8_t x = 0; x < strlen (str); x++)
//         {
//             rt += ((int)str[x]);
//         }
//     rt += (LARGE_CONSTANT * strlen (str));
//     return rt;
// }
// void
// getKeywordsId ()
// {
//     for (__uint8_t i = 0; i < KEYWORDS_LENGTH; i++)
//         {
//             keywordsID[i] = getId (keywords[i]);
//         }
// }
// y =0.00535x+2.958
Model::Model (const char *localPath)
{
    char buffer[512];
    unsigned int fileLenght, bufferLenght = 0;
    char *file = readFile (localPath, &fileLenght);
    for (int i = 0; i < fileLenght; i++)
        {
            if (file[i] == '#')
                {
                    i++;
                    while (file[i] != '\n')
                        i++;
                    continue;
                }
            if (bufferLenght == 0)
                {
                    if (file[i + 1] == ' ')
                        {
                            if (file[i] == 'v')
                                {
                                    i += 2;
                                    unsigned int j = i;
                                    vec3 v;
                                    for (int k = 0; k < 3; k++)
                                        {
                                            unsigned int b = i;
                                            while (1)
                                                {
                                                    if (file[i] == ' '
                                                        || file[i] == '\n')
                                                        break;
                                                    i++;
                                                }
                                            char c = file[i];
                                            file[i] = 0;
                                            v[k] = (float)atof (file + b);
                                            file[i] = c;
                                            i++;
                                        }
                                    vertex.push_back (v);
                                    i--;
                                }
                            else if (file[i] == 'f')
                                {
                                    unsigned int first[3], prev[3], index = 0;
                                    i += 2;
                                    unsigned int k = i;
                                    while (file[k] != '\n')
                                        k++;
                                    while (i < k)
                                        {
                                            // começa o for no index do
                                            // primeiro valor
                                            unsigned int value[3];
                                            char c;
                                            for (int j = 0; j < 3; j++)
                                                {
                                                    unsigned int i0 = i;
                                                    i++;
                                                    while (file[i] != '/'
                                                           && file[i] != ' '
                                                           && file[i] != '\n')
                                                        i++;
                                                    c = file[i];
                                                    file[i] = 0;
                                                    value[j]
                                                        = atoi (file + i0) - 1;
                                                    file[i] = c;
                                                    i++;
                                                }
                                            // i++;

                                            switch (index)
                                                {
                                                case 0:
                                                    first[0] = value[0];
                                                    first[1] = value[1];
                                                    first[2] = value[2];
                                                    break;
                                                case 1:
                                                    prev[0] = value[0];
                                                    prev[1] = value[1];
                                                    prev[2] = value[2];
                                                    break;
                                                default:
                                                
                                                    vertexIndex.push_back (
                                                        Index ((
                                                            unsigned int[3]){
                                                            first[0], prev[0],
                                                            value[0] }));
                                                    textureVertexIndex
                                                        .push_back (Index ((
                                                            unsigned int[3]){
                                                            first[1], prev[1],
                                                            value[1] }));
                                                    prev[0] = value[0];
                                                    prev[1] = value[1];
                                                    prev[2] = value[2];
                                                    break;
                                                }
                                            if (c == '\n')
                                                {
                                                    i--;
                                                    break;
                                                }
                                            index++;
                                        }
                                        printf("\n");
                                }
                        }
                    else if (file[i] == 'v' && file[i + 1] == 't'
                             && file[i + 2] == ' ')
                        {
                            i += 3;
                            unsigned int j = i;
                            vec2 v;
                            for (int k = 0; k < 2; k++)
                                {
                                    unsigned int b = i;
                                    while (1)
                                        {
                                            if (file[i] == ' '
                                                || file[i] == '\n')
                                                break;
                                            i++;
                                        }
                                    char c = file[i];
                                    file[i] = 0;
                                    v[k] = (float)atof (file + b);
                                    file[i] = c;
                                    i++;
                                }
                            texture.push_back (v);
                            i--;
                        }
                }
            else if (file[i] == '\n')
                {
                    if (bufferLenght == 0)
                        continue;
                }
        }
    free (file);
}
Model::~Model () {}
float *
Model::exportVertices (unsigned int *count)
{
    float *vert = (float *)malloc (3 * sizeof (float) * vertex.size ());
    for (int i = 0; i < vertex.size (); i++)
        {
            vert[3 * i] = vertex[i][0];
            vert[3 * i + 1] = vertex[i][1];
            vert[3 * i + 2] = vertex[i][2];
        }
    *count = 3 * vertex.size ();
    return vert;
}

float *
Model::exportTexture (unsigned int *count)
{
    float *tex = (float *)malloc (2 * sizeof (float) * texture.size ());
    for (int i = 0; i < texture.size (); i++)
        {
            tex[2 * i] = texture[i][0];
            tex[2 * i + 1] = texture[i][1];
        }
    *count = 2 * texture.size ();
    return tex;
}
unsigned int *
Model::exportVerticesIndex (unsigned int *count)
{
    *count = vertexIndex.size ()*3;
    return (unsigned int *)vertexIndex.data ();
}
unsigned int *
Model::exportTextureIndex (unsigned int *count)
{
    *count = textureVertexIndex.size ()*4;
    unsigned int *rt = (unsigned int *)malloc(sizeof(unsigned int) * *count);
    for(int i = 0; i < textureVertexIndex.size(); i++){
        rt[4*i] = textureVertexIndex.data()[i].v[0];
        rt[4*i+1] = textureVertexIndex.data()[i].v[1];
        rt[4*i+2] = textureVertexIndex.data()[i].v[2];
        rt[4*i+3] = 0;
    }
    return rt;
}