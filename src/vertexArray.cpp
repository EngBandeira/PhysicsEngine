#include "vertexArray.hpp"
#include "utils.hpp"
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
    glVertexAttribPointer (layoutIndex, componentsNumber, GL_FLOAT, GL_FALSE,
                           componentsNumber * sizeof (float),
                           (void *)0); // set vec4 position  if()
    glEnableVertexAttribArray (layoutIndex);
    layoutIndex++;
}
VAO::VAO(){
    glGenVertexArrays (1, &name);
    if(name == 0){
      sendError ("cannot create VAO");
            exit (1);
    }

}