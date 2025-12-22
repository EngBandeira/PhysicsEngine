#include "model.hpp"
#include "texture.hpp"
#include <cstddef>
#include <stdio.h>
#include <string.h>

#define BUFFER_LENGHT 64
#define KEYWORDS_LENGTH 8

template <typename T>
T *exportvec(unsigned int *count, std::vector<T> vec)
{
  *count = vec.size();
  T *rt = (T *)malloc(sizeof(T) * (*count));
  memcpy(rt,vec.data(),sizeof(T) * (*count));
  return rt;
}




void Mesh::deleteMesh(){
    free(vertices);
    free(textureVertices);
    free(normalVec);
    free(verticesIndex);
    free(textureVerticesIndex);
    free(normalIndex);
    // free(meshPath);
}

Mesh::Mesh(const char *meshPath): matrix(1),meshPath(meshPath)
{



    std::vector<float> vertices_,textureVertices_,normalVec_;
    std::vector<unsigned int> verticesIndex_, textureIndex_,normalIndex_;
    unsigned int fileLenght, bufferLenght = 0;
    char *file = readFile(meshPath, &fileLenght);
    for (unsigned int i = 0; i < fileLenght-1; i++) {
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
                for (int k = 0; k < 3; k++)
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
                    vertices_.push_back((float)atof(file + b));
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
                            verticesIndex_.push_back(first[0]);
                            verticesIndex_.push_back(prev[0]);
                            verticesIndex_.push_back(value[0]);

                            textureIndex_.push_back(first[1]);
                            textureIndex_.push_back(prev[1]);
                            textureIndex_.push_back(value[1]);
                            textureIndex_.push_back(0);

                            normalIndex_.push_back(value[2]);

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
            }
        } else if (file[i] == 'v' && file[i + 1] == 't' && file[i + 2] == ' ')
        {
            i += 3;
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
                textureVertices_.push_back((float)atof(file + b));
                file[i] = c;
                i++;
            }
            i--;
        }
        else if (file[i] == 'v' && file[i + 1] == 'n' && file[i + 2] == ' ')
        {
            i += 3;
            for (int k = 0; k < 3; k++)
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
                normalVec_.push_back((float)atof(file + b));
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
    normalVec = exportvec(&normalVecCount,normalVec_);
    normalIndex = exportvec(&verticesCount,normalIndex_);
    vertices = exportvec<float>(&verticesCount, vertices_);
    textureVertices = exportvec<float>(&textureVerticesCount, textureVertices_);
    verticesIndex = exportvec<unsigned int>(&verticesIndexCount, verticesIndex_);
    textureVerticesIndex = exportvec<unsigned int>(&textureVerticesIndexCount, textureIndex_);
    free(file);


}

Mesh::~Mesh()
{
    // printf("print\n");
    // free(vertices);
    // free(textureVertices);
    // free(normalVec);

    // free(verticesIndex);
    // free(textureIndex);
    // free(normalIndex);
}
