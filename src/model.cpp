#include "model.hpp"
#include <stdio.h>
#include <string.h>

#define BUFFER_LENGHT 64
#define KEYWORDS_LENGTH 8

template <typename T>
T *exportvec(unsigned int *count,
                                  std::vector<T> vec) {
  *count = vec.size();
  T *rt = (T *)malloc(sizeof(T) * *count);
  memcpy(rt,vec.data(),sizeof(T) * *count);

  return rt;
}


Mesh::Mesh(const char *localPath) {
    std::vector<float> vertex;
    std::vector<float> texture;
    std::vector<unsigned int> vertexIndex, textureVertexIndex;
    unsigned int fileLenght, bufferLenght = 0;
    char *file = readFile(localPath, &fileLenght);
    for (unsigned int i = 0; i < fileLenght; i++) {
        if (file[i] == '#') {
            i++;
            while (file[i] != '\n')
                i++;
                continue;
        }
        if (bufferLenght == 0) {
        if (file[i + 1] == ' ')
        {
            if (file[i] == 'v')
            {
                i += 2;
                unsigned int j = i;
                for (int k = 0; k < 3; k++)
                {
                    unsigned int b = i;
                    while (1) {
                    if (file[i] == ' ' || file[i] == '\n')
                        break;
                    i++;
                    }
                    char c = file[i];
                    file[i] = 0;
                    vertex.push_back((float)atof(file + b));
                    file[i] = c;
                    i++;
                }
                i--;
            }
            else if (file[i] == 'f')
            {
                // Here we have faces like this "f 1/1/1 5/2/1 7/3/1 3/4/1"
                // we will get the first num of the block and combine it like it
                // FIRST PREV ACTUAL, until we finish the face
                // in that case would be two triangles:(1 5 7), (1 7 3)
                unsigned int first[3], prev[3], index = 0;
                i += 2; // file[i] = f file[i+1] = ' '
                unsigned int k = i;
                while (file[k] != '\n')
                    k++;
                while (i < k)
                {

                    unsigned int value[3];
                    char c;
                    for (int j = 0; j < 3; j++)
                    {
                        unsigned int i0 = i; //Saving the actual index
                        i++;
                        while (file[i] != '/' && file[i] != ' ' && file[i] != '\n')
                            i++;
                        c = file[i];
                        file[i] = 0;
                        value[j] = atoi(file + i0) - 1;
                        file[i] = c;
                        i++;
                    }

                    switch (index)
                    {
                        case 0://Set the First
                            first[0] = value[0];
                            first[1] = value[1];
                            first[2] = value[2];
                        break;

                        case 1: // Set the Second point in the first batch
                            prev[0] = value[0];
                            prev[1] = value[1];
                            prev[2] = value[2];
                        break;

                        default: // From now on the first will be constant, but the prev and actual changing for each triangle
                            vertexIndex.push_back(first[0]);
                            vertexIndex.push_back(prev[0]);
                            vertexIndex.push_back(value[0]);

                            textureVertexIndex.push_back(first[1]);
                            textureVertexIndex.push_back(prev[1]);
                            textureVertexIndex.push_back(value[1]);
                            textureVertexIndex.push_back(0);
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
        } else if (file[i] == 'v' && file[i + 1] == 't' && file[i + 2] == ' ')
        {
            i += 3;
            unsigned int j = i;
            for (int k = 0; k < 2; k++)
            {
                unsigned int b = i;
                while (1)
                {
                    if (file[i] == ' ' || file[i] == '\n')
                        break;
                    i++;
                }
                char c = file[i];
                file[i] = 0;
                texture.push_back((float)atof(file + b));
                file[i] = c;
                i++;
            }
            i--;
        }
        } else if (file[i] == '\n')
        {
            if (bufferLenght == 0)
                continue;
        }
    }

    vertices = exportvec<float>(&verticesCount, vertex);
    textureVertices = exportvec<float>(&textureVerticesCount, texture);
    verticesIndex = exportvec<unsigned int>(&verticesIndexCount, vertexIndex);
    textureIndex = exportvec<unsigned int>(&textureIndexCount, textureVertexIndex);

    free(file);
}
Mesh::~Mesh() {
    free(vertices);
    free(textureVertices);
    // free(verticesIndex);//er
    free(textureIndex);
}
