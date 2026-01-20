#include "game_object.hpp"
#include "render.hpp"

#include <X11/X.h>
#include <cstdlib>
#include <glm/common.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/exponential.hpp>
#include <unistd.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <stdlib.h>

#include <GLFW/glfw3.h>
#include "vendor/glad/glad.h"

int fodase[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

Render::Render(GLFWwindow *win) : glfwWin(win) {

    render_data.flags = &flags;
    shaderProgram = glCreateProgram();
    shader.attach(shaderProgram);

    if( transFeed ) {
        const GLchar *feedbackVaryings[] = { "transformFeedback" };
        glTransformFeedbackVaryings(shaderProgram, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

        feedbacknumber = 4 * render_data.compacted_meshes.verticesIndexCount;
        feedbacksize = feedbacknumber * sizeof(float);

        glGenQueries(1, &QUERY);
        glGenBuffers(1, &TBO);
    }

    objects = (GameObject*)malloc(0);

    glLinkProgram(shaderProgram);
    utils.get_program_status(shaderProgram,GL_LINK_STATUS);
    glUseProgram(shaderProgram);

    glGenFramebuffers(1, &FBO_FROM);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_FROM);

    //texToRenderOver
    glGenTextures(1, &texToRenderOver);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texToRenderOver);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,samples,GL_RGB8 , 1920, 1080,GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texToRenderOver, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER,samples ,GL_DEPTH24_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &FBO_TO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_TO);

    //texToShow
    glGenTextures(1, &texToShowFrom);
    glBindTexture(GL_TEXTURE_2D, texToShowFrom);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    setTexParameter(GL_TEXTURE_2D);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texToShowFrom, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glUniform1iv(glGetUniformLocation(shaderProgram, "textures"), TEXTURE_HANDLERS_COUNT, fodase);
    // camera.updateMatrix(glm::vec3(1,1,1), utils.aim_matrix(-glm::vec3(1,1,1)));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera.viewMatrix));
}

unsigned int Render::addObject(GameObjectGenData genData) {
    objects = (GameObject*)realloc(objects, ++objects_count * sizeof(GameObject));
    objects[objects_count - 1].name = genData.name;
    return objects_count - 1;
}


Render::~Render() {
    render_data.freeData();

    if( transFeed ) {
        glDeleteQueries(1,&QUERY);
        glDeleteBuffers(1, &TBO);
    }

    glDeleteFramebuffers(1, &FBO_FROM);
    glDeleteFramebuffers(1, &FBO_TO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteTextures(1,&texToRenderOver);
    glDeleteTextures(1,&texToShowFrom);
    glDeleteProgram(shaderProgram);
}
