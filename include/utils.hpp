#pragma once
#include <glm/glm.hpp>

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

class Utils {
public:
    char *read_file(const char *localPath, unsigned int *fileLenght, const char *flags = "r");




    unsigned short get_old_index_of_new(unsigned short i, unsigned short n, unsigned short *index);
    unsigned short get_new_index_of_old(unsigned short i, unsigned short n, unsigned short *index);

    unsigned int get_extension_index(char *str,unsigned int size);

    void get_shader_status(unsigned int shaderProgram,int status);

    void get_program_status(unsigned int shaderProgram,int status);

    // Utils();

    template<typename T, size_t N>
    bool match_pairs(T *buffer, int j, const T (&pairs)[N]) {
        for(size_t i = 0; i < N; i++) {
            if(buffer[i+j] != pairs[i]) return false;
        }
        return true;
    }
};
