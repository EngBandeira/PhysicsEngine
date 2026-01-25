#include "common.hpp"
#include "vendor/glad/glad.h"

#include "shader.hpp"
#include "utils.hpp"

void Shader::init(const char *vertex_shaders, const char *geometry_shaders, const char *fragment_shaders) {
    const char *shadersPath[3] = {VERTEX_SHADERS_LOCALPATH, GEOMETRY_SHADERS_LOCALPATH, FRAGMENT_SHADERS_LOCALPATH};
    unsigned int sets[3]
        = { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER };
    int rt;
    for (int i = 0; i < 3; i++)
    {
        // if(i == 3)
        // break;
        char *file = utils.read_file (shadersPath[i], nullptr);
        shaders[i] = glCreateShader (sets[i]);
        glShaderSource (shaders[i], 1, &file, NULL);//aq
        glCompileShader (shaders[i]);
        free (file);

        glGetShaderiv (shaders[i], GL_COMPILE_STATUS, &rt);

        if (!rt) {
            glGetShaderiv (shaders[i],
                            GL_INFO_LOG_LENGTH,
                            &rt);
            char *info = (char *) malloc(rt);
            logger.sendError(info);
            free (info);
            exit (EXIT_FAILURE);
        }
    }
}
void Shader::attach (unsigned int program)
{
    glAttachShader (program, shaders[0]);
    glAttachShader (program, shaders[1]);
    glAttachShader (program, shaders[2]);
}
void Shader::free_data()
{
    glDeleteShader (shaders[0]);
    glDeleteShader (shaders[1]);
    glDeleteShader (shaders[2]);
}
