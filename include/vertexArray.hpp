#pragma once
#include "vendor/glad/glad.h"
#include <memory>
#include <vector>

class VBO {
    public:
    unsigned int name = 0;
    VBO ();
    ~VBO();
    VBO(VBO&& other) noexcept : name(other.name) {
        other.name = 0; // evita deleção dupla
    }

    // deletar copy
    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    // permitir move
    VBO& operator=(VBO&& other) noexcept {
        if (this != &other) {
            if (name) glDeleteBuffers(1, &name);
            name = other.name;
            other.name = 0;
        }
        return *this;
    }
};

class VAO {
      public:
        unsigned int name = 0, bindedVBO;
        unsigned short layoutIndex = 0;

        std::vector<std::unique_ptr<VBO>> vbos;

        void addLayout (unsigned short vboIndex,
                        unsigned char componentsNumber);
        void createVBO();
        void unbindVBO();
        void bindVBO(unsigned short vboIndex);
        void bind();
        void unbind();
        VAO ();
        ~VAO();
};
