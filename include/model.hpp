#pragma once
#include <glm/ext/matrix_float4x4.hpp>

#include "mesh.hpp"


struct ModelGenData {
    MeshGenData mesh;
    unsigned int materialIndex;
};

class Model {
    public:
    glm::vec3 axis[3];
    glm::mat4 rotationM,translationM,scaleM,matrix;
    glm::vec3 angle;
    Mesh mesh;
    unsigned int materialIndex;
    void scale(glm::vec3 scale);// scale is always universal
    void rotate(glm::vec3 rotation);
    void setAngle(glm::vec3 angle_);
    void translate(glm::vec3 translation);
    void positionate(glm::vec3 position);
    glm::vec3 getScale();
    glm::vec3 getAngle();
    glm::vec3 getPosition();
    Model(){};
    Model(Mesh mesh,unsigned int materialIndex);
};
