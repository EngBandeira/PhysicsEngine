#include <X11/X.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include "model.hpp"
#include "common.hpp"
#include "utils.hpp"

#define BUFFER_LENGHT 64
#define KEYWORDS_LENGTH 8

Model::Model(Mesh mesh,unsigned int materialIndex): rotationM(1),translationM(1),scaleM(1),matrix(1),angle(0),mesh(mesh),materialIndex(materialIndex) {
    axis[0] = glm::vec3(1,0,0);
    axis[1] = glm::vec3(0,1,0);
    axis[2] = glm::vec3(0,0,1);
}

void Model::scale(glm::vec3 scale){
    scaleM = utils.getScaleMatrix(scale);

    matrix =  translationM * rotationM * scaleM;
}

void Model::setAngle(glm::vec3 angle_degrees) {
    glm::vec3 angle_ = glm::radians(angle_degrees);


    glm::vec3 diff = angle_ - angle;

    if(diff[0] == 0 && diff[1] == 0  && diff[2] == 0 ) {
        return;
    }

    angle = angle_;

    rotationM = utils.getAngleMatrix(angle);

    matrix =  translationM * rotationM * scaleM;
}

void Model::rotate(glm::vec3 rotation_degrees) {
    glm::vec3 rotation = glm::radians(rotation_degrees);
    if(rotation[0] == 0 && rotation[1] == 0  && rotation[2] == 0 ) {
        return;
    }
    angle += rotation;

    rotationM = utils.getAngleMatrix(angle);

    matrix =  translationM * rotationM * scaleM;
}
void Model::translate(glm::vec3 translation){
    translationM[3][0] += translation.x;
    translationM[3][1] += translation.y;
    translationM[3][2] += translation.z;

    matrix =  translationM * rotationM * scaleM;
}
void Model::positionate(glm::vec3 position){
    translationM = utils.getPositionMatrix(position);

    matrix =  translationM * rotationM * scaleM;
}
glm::vec3 Model::getScale(){
    return glm::vec3(scaleM * glm::vec4(1));
}
glm::vec3 Model::getAngle(){
    glm::vec3 ret(glm::degrees(angle));
    return ret;
}
glm::vec3 Model::getPosition() {
    return glm::vec3(translationM * glm::vec4(0,0,0,1));
}
