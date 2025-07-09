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
model::model (const char *localPath)
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
                                                        = atoi (file + i0)-1;
                                                    file[i] = c;
                                                    i++;
                                                }
                                            printf (" %d/%d/%d ", value[0] + 1,
                                                    value[1] + 1,
                                                    value[2] + 1);

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
                                                        Index( (unsigned int[3]){
                                                            first[0], prev[0],
                                                            value[0] }));
                                                    textureVertexIndex
                                                        .push_back (Index((
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
                                    printf ("\n");
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
model::~model ()
{

}
