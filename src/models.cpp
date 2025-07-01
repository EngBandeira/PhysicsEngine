#include "models.hpp"
#include <stdio.h>
#include <string.h>

#define BUFFER_LENGHT 64
#define KEYWORDS_LENGTH 8

#define LARGE_CONSTANT 100000

const char *keywords[KEYWORDS_LENGTH]
    = { "v", "vn", "vt", "f", "o", "s", "mtllib", "usemtl" };
unsigned int keywordsID[KEYWORDS_LENGTH];

unsigned int
getId (const char *str)
{
    unsigned int rt = 0;
    for (__uint8_t x = 0; x < strlen (str); x++)
        {
            rt += ((int)str[x]);
        }
    rt += (LARGE_CONSTANT * strlen (str));
    return rt;
}
void
getKeywordsId ()
{
    for (__uint8_t i = 0; i < KEYWORDS_LENGTH; i++)
        {
            keywordsID[i] = getId (keywords[i]);
        }
}
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
                                    unsigned int first, prev, index = 0;
                                    i += 2;
                                    unsigned int k = i;
                                    while (file[k] != '\n')
                                        k++;
                                    for (int g = i; g < k; g++)
                                        { // começa o loop no primeiro  1/14/16
                                          // no 1
                                            unsigned int beta = g;
                                            g++;
                                            while (file[g] != '/')
                                                g++;
                                            file[g] = 0;
                                            unsigned int rt = atoi(file + beta);
                                            while (1)
                                                {
                                                    if (g >= k)
                                                        break;
                                                    if (file[g] != ' ')
                                                        {
                                                            g++;
                                                            continue;
                                                        }
                                                    break;
                                                }
                                            if (index == 0)
                                                {
                                                    first = rt;
                                                    index++;
                                                    continue;
                                                }
                                            else if (index == 1)
                                                {
                                                    prev = rt;
                                                    index++;
                                                    continue;
                                                }
                                            triangle tr ((unsigned int[3]){first, prev, rt});
                                            triangles.push_back (tr);
                                            prev = rt;
                                            // g++;
                                        }
                                }
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
    // free(vertex);
}

// while ((c = fgetc (file)) != EOF)
//         {
//             if (c == '#' && bufferIndex == 0)
//                 {
//                     while (fgetc (file) != '\n')
//                         ;
//                     continue;
//                 }
//             if (c == '\n')
//                 {
//                     char command[16];
//                     int i;
//                     for (i = 0; i < 16; i++)
//                         {
//                             if (buffer[i] == ' ')
//                                 break;
//                             command[i] = buffer[i];
//                         }
//                     command[i] = 0; // v = command[i-1]
//                     // std::cout << command << '\n';
//                     unsigned int id = getId (command);
//                     if (id == keywordsID[0])
//                         {
//                             int k = i + 1;
//                             vec3 result;
//                             for (int j = 0; j < 3; j++)
//                                 {
//                                     int w = k;
//                                     for (; k < bufferIndex; k++)
//                                         {
//                                             if (buffer[k] == ' ')
//                                                 break;
//                                         }
//                                     buffer[k] = 0;
//                                     result[i] = (float)atof(buffer);
//                                     buffer[k]= ' ';
//                                     k++;
//                                 }
//                             vertex.push_back(result);
//                         }
//                         else if(id == keywordsID[3]){
//                             int k = i + 1;
//                             int j = 0;
//                             int buf[100];
//                             while(k < bufferIndex)
//                                 {
//                                     int w = k;
//                                     for (; k < bufferIndex; k++)
//                                         {
//                                             if (buffer[k] == '/')
//                                                 break;
//                                         }
//                                     buffer[k] = 0;
//                                     std ::cout << buffer+w<<'\n';
//                                     buffer[k]= '/';
//                                     k++;
//                                     while(buffer[k] != ' '){
//                                         k++;
//                                     }
//                                     k++;
//                                     j++;
//                                 }
//                                 std::cout << '\n';
//                         }
//                     bufferIndex = 0;
//                     continue;
//                 }
//             if (bufferIndex < BUFFER_LENGHT)
//                 {
//                     buffer[bufferIndex++] = c;
//                 }
//             else
//                 {
//                     char message[64];
//                     sprintf (message, "line too big on file:%s", localPath);
//                     sendError (message);
//                     exit (1);
//                 }
//         }
