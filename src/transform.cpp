// #include "transform.hpp"
// #include "common.hpp"
// #include "utils.hpp"
// #include <stdio.h>

// #include <glm/ext/matrix_transform.hpp>
// #include <glm/trigonometric.hpp>

// #include <glm/ext/matrix_transform.hpp>
// #include <glm/fwd.hpp>
// #include <bits/stdc++.h>
// #include <stdio.h>

// #include "vendor/glad/glad.h"
// #include <GLFW/glfw3.h>

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

// Transform::Transform(){
//     axis[0] = glm::vec3(1,0,0);
//     axis[1] = glm::vec3(0,1,0);
//     axis[2] = glm::vec3(0,0,1);
// }

// glm::mat4 Transform::get_scale_matrix(glm::vec3 scale){
//     glm::mat4 scaleM(1);
//     scaleM[0][0] = scale.x;
//     scaleM[1][1] = scale.y;
//     scaleM[2][2] = scale.z;
//     return scaleM;
// }

// glm::mat4 Transform::get_angle_matrix(glm::vec3 angle_radians) {
//     glm::mat4 rotationM = glm::rotate(glm::mat4(1), angle_radians.x, axis[0]);
//     rotationM = glm::rotate(rotationM, angle_radians.y, axis[1]);
//     rotationM = glm::rotate(rotationM, angle_radians.z, axis[2]);
//     return rotationM;
// }


// glm::mat4 Transform::get_position_matrix(glm::vec3 position){
//     glm::mat4 translationM(1);
//     translationM[3][0] = position.x;
//     translationM[3][1] = position.y;
//     translationM[3][2] = position.z;
//     return translationM;
// }

// glm::mat4 Transform::aim_matrix(glm::vec3 direction){
//     glm::vec3 normali = glm::normalize(direction);
//     float theta = -atan(normali.z / normali.x);
//     if(normali.x < 0){
//         theta += glm::pi<float>();
//     }
//     float phi = asin(normali.y);
//     printf("amin: %f %f\n", glm::degrees(theta),glm::degrees(phi));
//     return get_angle_matrix(glm::vec3(0,theta,phi));
// }

// glm::mat4 *Transform::get_matrix(unsigned short index, unsigned int layerIndex){
//     return (glm::mat4*) (layers[layerIndex].matrices + 16*index);
// }

// void Transform::scale(glm::vec3 scale, unsigned short index, unsigned int layerIndex) {
//     layers[layerIndex].models[index].scale(scale);

//     *getNMatrix(index, layerIndex) = layers[layerIndex].models[index].matrix;
//     *flags = *flags | MATRICES_CHANGE_FLAG;
// }

// void Transform::scale(float scale, unsigned short index, unsigned int layerIndex) {
//     scaleModel(glm::vec3(scale), index, layerIndex);
// }

// void Transform::translate(glm::vec3 translation, unsigned short index, unsigned int layerIndex) {
//     layers[layerIndex].models[index].translate(translation);

//     *getNMatrix(index, layerIndex) = layers[layerIndex].models[index].matrix;
//     *flags = *flags | MATRICES_CHANGE_FLAG;
// }

// void Transform::rotate(glm::vec3 rotation, unsigned short index, unsigned int layerIndex) {
//     layers[layerIndex].models[index].rotate(rotation);

//     *getNMatrix(index, layerIndex) = layers[layerIndex].models[index].matrix;
//     *flags = *flags | MATRICES_CHANGE_FLAG;
// }

// void Transform::set_angle(glm::vec3 angle, unsigned short index, unsigned int layerIndex) {
//     layers[layerIndex].models[index].setAngle(angle);

//     *getNMatrix(index, layerIndex) = layers[layerIndex].models[index].matrix;
//     *flags = *flags | MATRICES_CHANGE_FLAG;
// }

// void Transform::positionate(glm::vec3 position, unsigned short index, unsigned int layerIndex) {
//     layers[layerIndex].models[index].positionate(position);

//     *getNMatrix(index, layerIndex) = layers[layerIndex].models[index].matrix;
//     *flags = *flags | MATRICES_CHANGE_FLAG;
// }

// glm::vec4 Transform::get_position(unsigned short index, unsigned int layerIndex) {
//     return *getNMatrix(index, layerIndex) * glm::vec4(0, 0, 0, 1);
// }
