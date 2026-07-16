#include "butia.hpp"

#include "draw_group/draw_group_mananger.hpp"
#include "material/material_mananger.hpp"
#include "utils/common.hpp"

#include "mesh/mesh.hpp"
#include "scene.hpp"
void Butia::init_gl_features() {
    glGenFramebuffers(1, &ante_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ante_fbo);

    //texToRenderOver
    glGenTextures(1, &ante_tex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ante_tex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,samples,GL_RGBA8 , 1920, 1080,GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ante_tex, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER,samples ,GL_DEPTH24_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &post_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, post_fbo);

    //texToShow
    glGenTextures(1, &post_tex);
    glBindTexture(GL_TEXTURE_2D, post_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    utils.setTexParameter(GL_TEXTURE_2D);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, post_tex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    glGenTextures(1, &tex_post_processed);
    glBindTexture(GL_TEXTURE_2D, tex_post_processed);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    utils.setTexParameter(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);


    post_processing_program = glCreateProgram();



    char *file = utils.read_file ("src/shaders/post.frag", nullptr);
    unsigned int compiled_shader = glCreateShader (GL_COMPUTE_SHADER);
    glShaderSource (compiled_shader, 1, &file, NULL);//aq
    glCompileShader (compiled_shader);

    free (file);

    utils.get_shader_status(compiled_shader, GL_COMPILE_STATUS);

    glAttachShader (post_processing_program, compiled_shader);

    glLinkProgram(post_processing_program);
    utils.get_program_status(post_processing_program,GL_LINK_STATUS);
}

Butia::Butia(GLFWwindow* window): window(window){
    butia = this;
    init_gl_features();
    draw_group_manager.init();
    texture_manager.init();
    shaders_manager.init();
    material_manager.init();


#ifdef TRANSFORM_FEEDBACK_ENABLE
        const GLchar *feedbackVaryings[] = { "transformFeedback" };
        glTransformFeedbackVaryings(shaderProgram, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

        feedbacknumber = 4 * render_data.compacted_meshes.index_count;
        feedbacksize = feedbacknumber * sizeof(float);

        glGenQueries(1, &QUERY);
        glGenBuffers(1, &TBO);
#endif

    objects = (GameObject*)malloc(0);


    render_data.init();

}

int Butia::new_frame() {

    Scene::newframe();
    Scene::draw();
    // Scene::post_processing();
    // Scene::ui();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return 0;
}
void Butia::use(Program program) {
    shader_program = program;
    glUseProgram(program.program);
}

unsigned int Butia::addObject(const char* mesh, const char* name, unsigned int draw_group) {
    return addObject(GameObjectGenData {
        .name = (char*)name,
        .uuid = (char*)"a",
        .mesh_index = render_data.add_mesh(MeshGenData {
            .path = (char*)mesh
        })
    }, draw_group);
}


unsigned int Butia::addObject(GameObjectGenData genData, unsigned int draw_group) {
    unsigned int rt = addObject(genData);
    draw_group_manager.groups[draw_group].addObject(rt);
    objects[rt].draw_group = draw_group;
    return rt;
}


unsigned int Butia::addObject(GameObjectGenData genData) {
    objects = (GameObject*)realloc(objects, ++objects_count * sizeof(GameObject));
    GameObject *obj = objects + objects_count - 1;
    *obj = GameObject();
    obj->name = genData.name;
    if(genData.mesh_index != (unsigned int)-1){
        obj->link_mesh(genData.mesh_index);
    }
    return objects_count - 1;
}

void Butia::terminate() {
    render_data.free_data();

#ifdef TRANSFORM_FEEDBACK_ENABLE
        glDeleteQueries(1,&QUERY);
        glDeleteBuffers(1, &TBO);
#endif

    glDeleteProgram(post_processing_program);

    glDeleteFramebuffers(1, &ante_fbo);
    glDeleteFramebuffers(1, &post_fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1,&ante_tex);
    glDeleteTextures(1,&post_tex);
    glDeleteTextures(1,&tex_post_processed);
    draw_group_manager.free_data();
}
