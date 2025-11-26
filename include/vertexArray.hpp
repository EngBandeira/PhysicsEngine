#pragma once
#include "glad/glad.h"
#include <vector>

class VBO
{
  public:
    unsigned int name = 0;
};

class VAO
{
    unsigned int name = 0;
    unsigned short layoutIndex = 0;
  public:
    std::vector<VBO> vbos;

    void addLayout (unsigned short vboIndex, unsigned char componentsNumber);
    inline void createVBO(){
      VBO v;
      // v.name = 
      vbos.push_back(v);
    }
    inline void
    unbindVBO ()
    {
        glBindBuffer (GL_ARRAY_BUFFER, 0);
    }
    inline void
    bindVBO (unsigned short vboIndex)
    {
        glBindBuffer (GL_ARRAY_BUFFER, vbos[vboIndex].name);
    }

    inline void
    bind ()
    {
        glBindVertexArray (name);
    }
    inline void
    unbind ()
    {
        glBindVertexArray (0);
    }
    VAO () {}
};