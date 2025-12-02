#include "vertexArray.hpp"
#include "utils.hpp"

VBO::VBO () { glGenBuffers (1, &name); }

void
VAO::addLayout (unsigned short vboIndex, unsigned char componentsNumber)
{
        if (vboIndex > vbos.size ())
                {
                        sendError ("vboIndex > vbos.size ()");
                        exit (1);
                }
        if (componentsNumber > 4 || componentsNumber == 0)
                {
                        sendError ("invalid componentsNumber");
                        exit (1);
                }
        glBindBuffer (GL_ARRAY_BUFFER, vbos[vboIndex].name);
        glVertexAttribPointer (layoutIndex, componentsNumber, GL_FLOAT,
                               GL_FALSE, componentsNumber * sizeof (float),
                               (void *)0); // set vec4 position  if()
        glEnableVertexAttribArray (layoutIndex);
        layoutIndex++;
}
VAO::VAO ()
{
        glGenVertexArrays (1, &name);
        if (name == 0)
                {
                        sendError ("cannot create VAO");
                        exit (1);
                }
}
void
VAO::createVBO ()
{
        VBO v;
        // v.name =
        vbos.push_back (v);
}
void
VAO::unbindVBO ()
{
        glBindBuffer (GL_ARRAY_BUFFER, 0);
        bindedVBO = 0;
}
void
VAO::bindVBO (unsigned short vboIndex)
{
        glBindBuffer (GL_ARRAY_BUFFER, vbos[vboIndex].name);
        bindedVBO = vboIndex;
}

void
VAO::bind ()
{
        glBindVertexArray (name);
}
void
VAO::unbind ()
{
        glBindVertexArray (0);
}