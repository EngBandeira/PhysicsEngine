#include <cstddef>
#include <cstdlib>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <stdio.h>
#include <string.h>

#include "model.hpp"
#include "utils.hpp"

#define BUFFER_LENGHT 64
#define KEYWORDS_LENGTH 8



template <typename T>
T *exportvec(unsigned int *count, std::vector<T> vec) {
  *count = vec.size();
  T *rt = (T *)malloc(sizeof(T) * (*count));
  memcpy(rt,vec.data(),sizeof(T) * (*count));
  return rt;
}

template<typename T, size_t N>
bool matchPairs(T *buffer, int j, const T (&pairs)[N]) {
    for(size_t i = 0; i < N; i++) {
        if(buffer[i+j] != pairs[i]) return false;
    }
    return true;
}



void Mesh::deleteMesh() {
    free(vertices);
    free(textureVertices);
    free(normalVertices);
    free(verticesIndex);
    free(textureVerticesIndex);
    free(normalVerticesIndex);
    free(mtl);
    for(int i = 0; i < oCount; i++){
        free(o[i]);
    }
    free(o);
    // free(meshPath);
}

Mesh::Mesh(MeshGenData genData): meshPath(genData.path) {

    o = (char**)malloc(0);
    std::vector<float> vertices_,textureVertices_,normalVertices_;
    std::vector<unsigned int> verticesIndex_, textureVerticesIndex_,normalVerticesIndex_;
    unsigned int fileLenght = 0;
    char *buffer = readFile(meshPath, &fileLenght);
    char c;
    unsigned int i = 0;
    while(1) {
        c = buffer[i];
        if(c == 0)
            break;
        if(c == '\n') {
            i++;
            continue;
        }
        if(c == '#') {
            while(buffer[i] != '\n')
                i++;
            i++;
            continue;
        }

        if( matchPairs(buffer, i, {'m', 't', 'l', 'l', 'i', 'b', ' '})) {
            i += 7;
            unsigned int k = i;
            while(buffer[i] != '\n' && buffer[i] != ' ')
                i++;
            mtl = (char*)malloc(i - k +1);
            memcpy(mtl, buffer+k, i - k);
            mtl[i - k] = 0;
            continue;
        }
        if( c == 'o' && buffer[i+1] == ' ') {
            i += 2;
            unsigned int k = i;
            while(buffer[i] != '\n' && buffer[i] != ' ')
                i++;
            oCount++;
            o = (char**)realloc(o,sizeof(char*) * oCount);
            o[oCount-1] = (char*)malloc(i - k +1);
            memcpy(o[oCount-1], buffer+k, i - k);
            o[oCount-1][i - k] = 0;
            continue;
        }
        if(c == 'v') {
            if(buffer[i+1] == ' ') {
                i += 2;
                for(int j = 0; j < 3; j++){
                    unsigned int k = i;
                    while(buffer[i] != '\n' && buffer[i] != ' ')
                        i++;
                    buffer[i] = 0;
                    vertices_.push_back((float)atof(buffer+ k));
                    buffer[i] = ' ';
                    i++;
                }
                continue;
            }
            if(buffer[i+1] == 'n' && buffer[i+2] == ' ' ) {
                i += 3;
                for(int j = 0; j < 3; j++){
                    unsigned int k = i;
                    while(buffer[i] != '\n' && buffer[i] != ' ')
                        i++;
                    buffer[i] = 0;
                    normalVertices_.push_back((float)atof(buffer+ k));
                    buffer[i] = ' ';
                    i++;
                }
                continue;
            }
            if(buffer[i+1] == 't' && buffer[i+2] == ' ' ) {
                i += 3;
                for(int j = 0; j < 2; j++){
                    unsigned int k = i;
                    while(buffer[i] != '\n' && buffer[i] != ' ')
                        i++;
                    buffer[i] = 0;
                    textureVertices_.push_back((float)atof(buffer+ k));
                    buffer[i] = ' ';
                    i++;
                }
                continue;
            }
        }
        if(c == 'f' && buffer[i+1] == ' ') {
            i += 2;
            unsigned int first[3], prev[3], value[3];
            unsigned int indexValues = 0;
            unsigned int k;
            while(1) {
                for(int j = 0; j < 3; j++) {
                    k = i;
                    while(buffer[i] != '/' && buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != 0)
                        i++;
                    char b = buffer[i];
                    buffer[i] = 0;
                    value[j] = atoi(buffer + k)-1;
                    buffer[i] = b;
                    i++;
                }
                switch (indexValues) {
                    case 0: {
                        first[0] = value[0];
                        first[1] = value[1];
                        first[2] = value[2];
                        break;
                    }
                    case 1: {
                        prev[0] = value[0];
                        prev[1] = value[1];
                        prev[2] = value[2];
                        break;
                    }
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
                    }
                }
                indexValues++;
                if( buffer[i-1] == '\n' || buffer[i-1] == 0){
                    i--;
                    break;
                }
            }
            continue;
        }
        if(c == 's' && buffer[i+1] == ' '){
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

Model::Model(Mesh mesh,unsigned int materialIndex):rotationM(1),translationM(1),scaleM(1),matrix(1),mesh(mesh),materialIndex(materialIndex){}
void Model::scale(glm::vec3 scale){
    scaleM[0][0] = scale.x;
    scaleM[1][1] = scale.y;
    scaleM[2][2] = scale.z;
    matrix = rotationM * translationM * scaleM;
}
void Model::rotate(float angl,AXIS axis){
    glm::vec3 ax(0);
    ax[axis] = 1;
    rotationM = glm::rotate(rotationM, glm::radians(angl),ax);
    angle[axis] += angl;
    matrix = rotationM * translationM * scaleM;
}
void Model::translate(glm::vec3 translation){
    translationM[3][0] += translation.x;
    translationM[3][1] += translation.y;
    translationM[3][2] += translation.z;
    matrix = rotationM * translationM * scaleM;
}
void Model::positionate(glm::vec3 position){
    translationM[3][0] = position.x;
    translationM[3][1] = position.y;
    translationM[3][2] = position.z;
    matrix = rotationM * translationM * scaleM;
}
glm::vec3 Model::getScale(){
    return glm::vec3(scaleM * glm::vec4(1));
}
glm::vec3 Model::getRotation(){
    return angle;
}
glm::vec3 Model::getPosition() {
    return glm::vec3(translationM * glm::vec4(0,0,0,1));
}
