
#include "render.hpp"
#include "utils.hpp"


void Render::post_processing() {
    glUseProgram(post_program);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D,texToShowFrom);
    glUniform1i(glGetUniformLocation(post_program,"texToShowFrom"),10);
    glBindImageTexture(0, tex_post, 0, GL_TRUE, 0, GL_READ_WRITE,  GL_RGBA8 );
    // glGen
    glDispatchCompute(120,68,1);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
}
