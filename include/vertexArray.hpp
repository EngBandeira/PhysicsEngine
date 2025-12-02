#pragma once
#include "glad/glad.h"
#include <vector>

class VBO
{
      public:
        unsigned int name = 0;
        VBO ();
};

class VAO
{
      public:
        unsigned int name = 0, bindedVBO;
        unsigned short layoutIndex = 0;

        std::vector<VBO> vbos;

        void addLayout (unsigned short vboIndex,
                        unsigned char componentsNumber);
        void createVBO ();
        void unbindVBO ();
        void bindVBO (unsigned short vboIndex);
        void bind ();
        void unbind ();
        VAO ();
};