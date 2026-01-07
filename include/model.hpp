#pragma once
#include <glm/ext/matrix_float4x4.hpp>

enum AXIS{
  X,
  Y,
  Z
};


struct MeshGenData {
    char *path;
};


class Mesh {
    public:
    // Ponto[verticeCount/3] =
    // {vertices[verticesCount],vertices[verticesCount+1],vertices[verticesCount+2]}
    // Ponto[textureVerticesCount/2] =
    // {textureVertices[textureVerticesCount],textureVertices[textureVerticesCount+1]}
    unsigned int oCount = 0;
    char *mtl;
    char **o;
    unsigned int verticesCount = 0, verticesIndexCount = 0, textureVerticesCount=0,
                 textureVerticesIndexCount=0,normalVerticesCount=0,verticesOffsetIndex= 0;// normalIndexCount = nOfPrimitives = verticesIndexCount /3
    float *vertices,*textureVertices,*normalVertices;
    unsigned int *verticesIndex,*textureVerticesIndex,*normalVerticesIndex;
    const char *meshPath;
    void deleteMesh();
    Mesh(){};
    Mesh(MeshGenData genData);
    ~Mesh();
};

class Model{
    public:
    glm::mat4 rotationM,translationM,scaleM,matrix;
    glm::vec3 angle;
    Mesh mesh;
    unsigned int materialIndex;
    void scale(glm::vec3 scale);// scale is always universal
    void rotate(float angl,AXIS axis);
    void translate(glm::vec3 translation);
    void positionate(glm::vec3 position);
    glm::vec3 getScale();
    glm::vec3 getRotation();
    glm::vec3 getPosition();
    Model(){};
    Model(Mesh mesh,unsigned int materialIndex);
};
