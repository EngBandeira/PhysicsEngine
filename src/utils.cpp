#include "common.hpp"
#include "utils.hpp"
#include <stdio.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>


char* Utils::readFile(const char *localPath, unsigned int *fileLenght, const char *flags) {
    FILE *file = fopen(localPath, flags);
    char *buffer;

    if( file == NULL ){
        char message[100];
        sprintf(message, "cannot open file %s", localPath);
        logger.sendError(message, 1);
    }

    fseek(file, 0L, SEEK_END);

    unsigned int sz = ftell(file) + 1;
    fseek(file, 0, SEEK_SET);

    buffer = (char *) malloc(sz * sizeof(char));
    buffer[sz - 2] = EOF;
    fread(buffer, sizeof(char), (sz - 1), file);

    fclose(file);

    if( fileLenght != nullptr ) *fileLenght = sz;
    buffer[sz - 1] = 0;
    return buffer;
}

Utils::Utils(){
    axis[0] = glm::vec3(1,0,0);
    axis[1] = glm::vec3(0,1,0);
    axis[2] = glm::vec3(0,0,1);
}

glm::mat4 Utils::getScaleMatrix(glm::vec3 scale){
    glm::mat4 scaleM(1);
    scaleM[0][0] = scale.x;
    scaleM[1][1] = scale.y;
    scaleM[2][2] = scale.z;
    return scaleM;
}

glm::mat4 Utils::getAngleMatrix(glm::vec3 angle_radians) {
    glm::mat4 rotationM = glm::rotate(glm::mat4(1), angle_radians.x, axis[0]);
    rotationM = glm::rotate(rotationM, angle_radians.y, axis[1]);
    rotationM = glm::rotate(rotationM, angle_radians.z, axis[2]);
    return rotationM;
}


glm::mat4 Utils::getPositionMatrix(glm::vec3 position){
    glm::mat4 translationM(1);
    translationM[3][0] = position.x;
    translationM[3][1] = position.y;
    translationM[3][2] = position.z;
    return translationM;
}

glm::mat4 Utils::aimMatrix(glm::vec3 direction){
    glm::vec3 normali = glm::normalize(direction);
    float theta = -atan(normali.z / normali.x);
    if(normali.x < 0){
        theta += glm::pi<float>();
    }
    float phi = asin(normali.y);
    printf("amin: %f %f\n",glm::degrees(theta),glm::degrees(phi));
    return utils.getAngleMatrix(glm::vec3(0,theta,phi));
}
