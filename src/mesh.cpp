#include <X11/X.h>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <string.h>

#include "mesh.hpp"
#include "common.hpp"


template <typename T>
T *exportvec(unsigned int *count, std::vector<T> vec) {
  *count = vec.size();
  T *rt = (T *) malloc(sizeof(T) * (*count));
  memcpy(rt,vec.data(),sizeof(T) * (*count));
  return rt;
}


Mesh::Mesh(MeshGenData genData): meshPath(genData.path){

    o = (char**)malloc(0);
    std::vector<float> vertices_,textureVertices_,normalVertices_;
    std::vector<unsigned int> verticesIndex_, textureVerticesIndex_,normalVerticesIndex_;
    unsigned int fileLenght = 0;
    char *buffer = utils.readFile(meshPath, &fileLenght);
    char c;
    unsigned int i = 0;
    while(1) {
        c = buffer[i];
        if( c == '\0' ) break;

        if( c == '\n' ) {
            i++;
            continue;
        }

        if( c == '#' ) {
            while(buffer[i] != '\n') i++;
            i++;
            continue;
        }

        if( utils.matchPairs(buffer, i, {'m', 't', 'l', 'l', 'i', 'b', ' '})) {
            i += 7;

            unsigned int k = i;
            while(buffer[i] != '\n' && buffer[i] != ' ') i++;

            mtl = (char*) malloc(i - k + 1);
            memcpy(mtl, (buffer  +k), (i - k));
            mtl[i - k] = 0;
            continue;
        }

        if( c == 'o' && buffer[i+1] == ' ') {
            i += 2;

            unsigned int k = i;
            while(buffer[i] != '\n' && buffer[i] != ' ') i++;

            oCount++;
            o = (char**) realloc(o, sizeof(char*) * oCount);
            o[oCount - 1] = (char*) malloc(i - k + 1);
            memcpy(o[oCount - 1], (buffer + k), (i - k));
            o[oCount - 1][i - k] = 0;
            continue;
        }

        if( c == 'v' ) {
            if( buffer[i+1] == ' ' ) {
                i += 2;

                for( int j = 0; j < 3; j++ ) {
                    unsigned int k = i;
                    while(buffer[i] != '\n' && buffer[i] != ' ') i++;

                    buffer[i] = 0;
                    vertices_.push_back((float) atof(buffer+ k));
                    buffer[i] = ' ';
                    i++;
                }

                continue;
            }

            if( buffer[i+1] == 'n' && buffer[i+2] == ' ') {
                i += 3;

                for( int j = 0; j < 3; j++ ) {
                    unsigned int k = i;
                    while(buffer[i] != '\n' && buffer[i] != ' ') i++;

                    buffer[i] = 0;
                    normalVertices_.push_back((float) atof(buffer+ k));
                    buffer[i] = ' ';
                    i++;
                }

                continue;
            }

            if( buffer[i+1] == 't' && buffer[i+2] == ' ' ) {
                i += 3;

                for( int j = 0; j < 2; j++ ) {
                    unsigned int k = i;
                    while(buffer[i] != '\n' && buffer[i] != ' ') i++;

                    buffer[i] = 0;
                    textureVertices_.push_back((float)atof(buffer+ k));
                    buffer[i] = ' ';
                    i++;
                }

                continue;
            }
        }

        if( c == 'f' && buffer[i+1] == ' ' ) {
            i += 2;

            unsigned int k,
                         indexValues = 0,
                         first[3],
                         prev[3],
                         value[3];

            while(1) {

                for( int j = 0; j < 3; j++ ) {
                    k = i;
                    while(buffer[i] != '/' && buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != 0) i++;

                    char b = buffer[i];
                    buffer[i] = 0;
                    value[j] = atoi(buffer + k)-1;
                    buffer[i] = b;
                    i++;
                }

                switch(indexValues) {
                    case 0: {
                        first[0] = value[0];
                        first[1] = value[1];
                        first[2] = value[2];
                    } break;
                    case 1: {
                        prev[0] = value[0];
                        prev[1] = value[1];
                        prev[2] = value[2];
                    } break;
                    default: {
                        verticesIndex_.push_back(first[0]);
                        verticesIndex_.push_back(prev[0]);
                        verticesIndex_.push_back(value[0]);

                        textureVerticesIndex_.push_back(first[1]);
                        textureVerticesIndex_.push_back(prev[1]);
                        textureVerticesIndex_.push_back(value[1]);

                        normalVerticesIndex_.push_back(first[2]);

                        prev[0] = value[0];
                        prev[1] = value[1];
                        prev[2] = value[2];
                    } break;
                }

                indexValues++;
                if( buffer[i-1] == '\n' || buffer[i-1] == 0 ) {
                    i--;
                    break;
                }
            }

            continue;
        }

        if( c == 's' && buffer[i + 1] == ' ' ) {
            i += 2;
            continue;
        }

        i++;
    }
    free(buffer);
    verticesIndex = exportvec<unsigned int>(&verticesIndexCount, verticesIndex_);
    textureVerticesIndex = exportvec<unsigned int>(&textureVerticesIndexCount, textureVerticesIndex_);
    normalVerticesIndex = exportvec<unsigned int>(&verticesCount, normalVerticesIndex_);

    vertices = exportvec<float>(&verticesCount, vertices_);
    textureVertices = exportvec<float>(&textureVerticesCount, textureVertices_);
    normalVertices = exportvec<float>(&normalVerticesCount, normalVertices_);
}

Mesh::~Mesh() {}

void Mesh::deleteMesh() {
    free(vertices);
    free(textureVertices);
    free(normalVertices);
    free(verticesIndex);
    free(textureVerticesIndex);
    free(normalVerticesIndex);
    free(mtl);
    for( int i = 0; i < oCount; i++ )free(o[i]);
    free(o);
    // free(meshPath);
}
