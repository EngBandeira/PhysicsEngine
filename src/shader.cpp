#include "glad/glad.h"
#include <string.h>

#include "shader.hpp"
#include "utils.hpp"

Shader::Shader (const char *shadersPath[3])
{
        unsigned int sets[3]
            = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER };
        int rt;
        for (int i = 0; i < 3; i++)
            {
                // if(i == 3)
                // break;
                char *file = readFile (shadersPath[i], nullptr);
                Shaders[i] = glCreateShader (sets[i]);
                glShaderSource (Shaders[i], 1, &file, NULL);
                glCompileShader (Shaders[i]);
                free (file);

                glGetShaderiv (Shaders[i], GL_COMPILE_STATUS, &rt);

                if (!rt)
                    {
                        glGetShaderiv (Shaders[i],
                                        GL_INFO_LOG_LENGTH,
                                        &rt);
                        char *info = (char *)malloc (rt);
                        sendError (info);
                        free (info);
                        exit (1);
                    }
            }
}
void
Shader::attach (unsigned int program)
{
    glAttachShader (program, Shaders[0]);
    glAttachShader (program, Shaders[1]);
    glAttachShader (program, Shaders[2]);
}
Shader::~Shader ()
{
    glDeleteShader (Shaders[0]);
    glDeleteShader (Shaders[1]);
    glDeleteShader (Shaders[2]);
}
