#include "common.hpp"
#include "utils.hpp"
#include "engine.hpp"
#include <stdio.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <bits/stdc++.h>
#include <stdio.h>

#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <openssl/sha.h>

unsigned int Utils::sha256(char* str) {

    unsigned char *hash = (unsigned char*)malloc(SHA256_DIGEST_LENGTH);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, strlen(str));
    SHA256_Final(hash, &sha256);

    unsigned int rt = 0;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        rt += hash[i];
    }

    return rt;
}

void Utils::get_shader_status(unsigned int shaderProgram,int status){
    int linkStatus;
    glGetShaderiv(shaderProgram, status, &linkStatus);
    if (!linkStatus) {
        GLint infoLogLength;
        glGetShaderiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *c = (char*)malloc(infoLogLength + 1);
        glGetShaderInfoLog(shaderProgram, infoLogLength, nullptr, c);
        c[infoLogLength] = 0;
        logger.sendError(c);
        free(c);
    }
}

void Utils::get_program_status(unsigned int shaderProgram,int status){
    int linkStatus;
    glGetProgramiv(shaderProgram, status, &linkStatus);
    if (!linkStatus) {
        GLint infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *c = (char*)malloc(infoLogLength + 1);
        glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, c);
        c[infoLogLength] = 0;
        logger.sendError(c);
        free(c);
    }
}

unsigned int Utils::get_last_c(char *str,unsigned int size, char c){
    unsigned int i = size - 2;
    while( str[i] != c && i > 0 ) i--;
    return i;
}

//Returns the index of the point on the extension
unsigned int Utils::get_extension_index(char *str,unsigned int size){
    return get_last_c(str, size, '.');
}

char* Utils::read_file(const char *localPath, unsigned int *fileLenght, const char *flags) {
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

unsigned short Utils::get_old_index_of_new(unsigned short i, unsigned short n, unsigned short *index) {
    unsigned short rt = i;
    for( unsigned short j = 0; j < n; j++ ) {
        if( index[j] <= rt ) rt++;
    }
    return rt;
}

unsigned short Utils::get_new_index_of_old(unsigned short i, unsigned short n, unsigned short *index) {
    unsigned short rt = i;
    for( unsigned short j = 0; j < n; j++ ) {
        if(index[j] == i){
            logger.sendError("Acess of Delete Index");
            return 0;
        }
        if(index[j] < i) rt--;
    }
    return  rt;
}
