#include "render.hpp"

#include <X11/X.h>
#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/trigonometric.hpp>
#include <cstdio>
#include <glm/geometric.hpp>
#include <glm/exponential.hpp>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <stdlib.h>
#include "common.hpp"

#include "vendor/glad/glad.h" //GLAD Always upper than GLFW
#include <GLFW/glfw3.h>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "model.hpp"
#include "shader.hpp"



unsigned int flags; // abcd efgh: h = Update renderData
int fodase[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

// template<typename T, size_t N>
// bool matchPairs(T *buffer, int j, const T (&pairs)[N]) {
//     for(size_t i = 0; i < N; i++) {
//         if(buffer[i+j] != pairs[i]) return false;
//     }
//     return true;
// }




void Render::getShaderStatus(unsigned int shaderProgram,int status){
    int linkStatus;
    glGetShaderiv(shaderProgram, status, &linkStatus);
    if (!linkStatus) {
        GLint infoLogLength;
        glGetShaderiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *c = (char*)malloc(infoLogLength + 1);
        glGetShaderInfoLog(shaderProgram, infoLogLength, nullptr, c);
        c[infoLogLength] = 0;
        logger.sendError(c);
        free(c);
    }
}

void Render::getProgramStatus(unsigned int shaderProgram,int status){
    int linkStatus;
    glGetProgramiv(shaderProgram, status, &linkStatus);
    if (!linkStatus) {
        GLint infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *c = (char*)malloc(infoLogLength + 1);
        glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, c);
        c[infoLogLength] = 0;
        logger.sendError(c);
        free(c);
    }
}





// rotation[0][0] = r.x; rotation[1][0] = r.y; rotation[2][0] = r.z;
// rotation[0][1] = u.x; rotation[1][1] = u.y; rotation[2][1] = u.z;
// rotation[0][2] = -f.x; rotation[1][2] = -f.y; rotation[2][2] = -f.z;
//



// 0123456789
// x01x23x456
unsigned short Render::getOldIndexOfNew(unsigned short i, unsigned short n,unsigned short *index){
    unsigned short rt = i;
    for (unsigned short j = 0; j < n; j++){
        if(index[j] <= rt){
            rt++;
        }
    }
    return rt;
}


unsigned short Render::getNewIndexOfOld(unsigned short i, unsigned short n,unsigned short *index){
    unsigned short rt = i;
    for (unsigned short j = 0; j < n; j++){
        if(index[j] == i){
            logger.sendError("Acess of Delete Index");
            return 0;
        }
        if(index[j] < i)
            rt--;
    }
    return  rt;
}

unsigned int vectorMaterialI;

unsigned int Render::setAVector(glm::vec3 position,glm::vec3 direction){
    unsigned int modelI = renderData.pushModels(MeshGenData {
        .path = (char*)"assets/3dmodels/Vector.obj"
    }, vectorMaterialI,LAYER::SPECIAL_LAYER);
    updatePipeline(LAYER::SPECIAL_LAYER);
    glm::vec3 normali = glm::normalize(direction);
    // float theta = acos(normali.z);
    // float dote = glm::dot(normali,glm::vec3(1,0,0));
    // float theta = acos(dote);
    float theta = -atan(normali.z / normali.x);
    if(normali.x < 0){
        theta += glm::pi<float>();
    }
    float phi = asin(normali.y);
    // printf("angle: %f \n",glm::degrees(theta));

    renderData.setAngleModel(glm::vec3(0,glm::degrees(theta),glm::degrees(phi)), modelI, LAYER::SPECIAL_LAYER);
    // renderData.rotateModel(glm::vec3(0,0,), modelI, LAYER::SPECIAL_LAYER);

    renderData.translateModel(position, modelI, LAYER::SPECIAL_LAYER);
    renderData.scaleModel(0.05, modelI, LAYER::SPECIAL_LAYER);
    flags = flags | MODELS_CHANGE_FLAG;
    return modelI;
}


//returns the index of the point on the extension
unsigned int getExtension(char *str,unsigned int size){
    unsigned int i = size - 2;
    while( str[i] != '.' && i > 0 ) i--;
    return i;
}

void Assets::update() {
    if( files.size() ) files.clear();
    if( directory == nullptr ) return;

    char *command = (char*) malloc(strlen(directory) + 19);
    sprintf(command, "ls %s -p > .assets", directory);
    pclose(popen(command, "r"));

    unsigned int fileLenght;
    char* buffer = utils.readFile(".assets", &fileLenght);
    unsigned int i = 0;
    while(1) {

        unsigned int k = i;
        while( buffer[i] != '\n' && buffer[i] != 0 ) i++;

        if( buffer[i] == 0 ) break;
        if( i == k ){
            i++;
            continue;
        }
        char *c = (char*) malloc(i - k + 1);
        memcpy(c, (buffer + k), (i - k));

        c[i - k] = 0;

        unsigned int exteIndex = getExtension(c, i - k + 1);
        File file;
        if( c[i - k - 1] == '/' ) file.type = FILE_TYPES::FOLDER_FILE;
        file.completeName = c;
        if( exteIndex == 0 ) {
            file.extension = nullptr;
            file.simpleName = c;
        }
        else {
            file.extension = (char*)malloc(i - k + 1 - exteIndex);
            memcpy(file.extension, c+exteIndex, i - k + 1 - exteIndex);
            file.extension[i - k - exteIndex] = 0;

            file.simpleName = (char*)malloc(exteIndex + 1);
            memcpy(file.simpleName, c, exteIndex);
            file.simpleName[exteIndex] = 0;
        }

        if( file.extension != nullptr && utils.matchPairs(file.extension, 1, {'o', 'b', 'j'}) ) file.type = MESH_FILE;
        files.push_back(file);
        i++;
    }
    File fParent;
    fParent.completeName = (char*)"../";
    fParent.type = FILE_TYPES::FOLDER_FILE;
    files.push_back(fParent);
    free(buffer);
}

Render::Render(GLFWwindow *win) : glfwWin(win) {

    renderData.flags = &flags;
    shaderProgram = glCreateProgram();
    shader.attach(shaderProgram);

    if( transFeed ) {
        const GLchar *feedbackVaryings[] = { "transformFeedback" };
        glTransformFeedbackVaryings(shaderProgram, 1, feedbackVaryings,
                                    GL_INTERLEAVED_ATTRIBS);

        feedbacknumber = 4 * renderData.layers[1].verticesIndexCount;
        feedbacksize = feedbacknumber * sizeof(float);
    }


    glLinkProgram(shaderProgram);
    getProgramStatus(shaderProgram,GL_LINK_STATUS);
    glUseProgram(shaderProgram);

}

Render::~Render() {
    renderData.freeData();
    if (transFeed) {
        glDeleteQueries(1,&QUERY);
        glDeleteBuffers(1, &TBO);
    }
    for( int i = 0; i < LAYERS_COUNT; i++ ) {
        glDeleteBuffers(1, &renderData.layers[i].ebo);
        glDeleteBuffers(VBO_COUNT,renderData.layers[i].vbos);
        glDeleteVertexArrays(1,&renderData.layers[i].vao);
        glDeleteBuffers(SSBO_PER_LAYER_COUNT,renderData.layers[i].ssbos);
    }
    glDeleteBuffers(1,&renderData.materialsSSBO);
    glDeleteBuffers(1,&renderData.lampsSSBO);

    glDeleteFramebuffers(1, &FBO_FROM);
    glDeleteFramebuffers(1, &FBO_TO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteTextures(1,&texToRenderOver);
    glDeleteTextures(1,&texToShowFrom);
    glDeleteProgram(shaderProgram);
}

MaterialGenData a;

void Render::once()
{
    if (transFeed) {
        glGenQueries(1, &QUERY);
        glGenBuffers(1, &TBO);
    }
    for(int i = 0; i < LAYERS_COUNT; i++){
        MeshRenderData *data = renderData.layers +i;
        glGenVertexArrays(1,&data->vao);
        glBindVertexArray(data->vao);

        glGenBuffers(VBO_COUNT,data->vbos);
        glBindBuffer(GL_ARRAY_BUFFER,data->vbos[0]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                                (void *)0);

        glBindBuffer(GL_ARRAY_BUFFER,data->vbos[1]);
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(int),
                                (void *)0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        glGenBuffers(1, &data->ebo);


        glGenBuffers(SSBO_PER_LAYER_COUNT, renderData.layers[i].ssbos);
        for(unsigned int j = 0; j < SSBO_PER_LAYER_COUNT; j++){
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderData.layers[i].ssbos[j]);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, j, renderData.layers[i].ssbos[j]);
        }
    }

    glGenBuffers(1, &renderData.materialsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderData.materialsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBOS::MaterialsSSBO, renderData.materialsSSBO);

    glGenBuffers(1, &renderData.lampsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderData.lampsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBOS::LampsSSBO, renderData.lampsSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                        GL_FALSE, glm::value_ptr(camera.projMatrix));


    glGenFramebuffers(1, &FBO_FROM);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_FROM);

    //texToRenderOver
    glGenTextures(1, &texToRenderOver);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texToRenderOver);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,samples,GL_RGB8 , 1920, 1080,GL_TRUE);//aa
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE,
                            texToRenderOver, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER,samples ,GL_DEPTH24_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                GL_RENDERBUFFER, RBO);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    glGenFramebuffers(1, &FBO_TO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_TO);

    //texToShow
    glGenTextures(1, &texToShowFrom);
    glBindTexture(GL_TEXTURE_2D, texToShowFrom);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB,
                GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            texToShowFrom, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glGenTextures(1, &renderData.texUtilitary);
    glBindTexture(GL_TEXTURE_2D_ARRAY, renderData.texUtilitary);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,0,0,0,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    setTexParameter();
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    for(int i = 0; i < TEXTURE_HANDLERS_COUNT; i++){
        unsigned short k = pow(2,6+i);
        renderData.textureHandlers[i].texturesCount = 0;
        renderData.textureHandlers[i].texUtilitary = &renderData.texUtilitary;
        glGenTextures(1,&renderData.textureHandlers[i].texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY,renderData.textureHandlers[i].texture);
        glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,k,k,0,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);

        setTexParameter();
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);


    glUniform1iv(glGetUniformLocation(shaderProgram, "textures"), TEXTURE_HANDLERS_COUNT,fodase);


    // allocMaterial(MaterialGenData{});


}


void Render::newframe()
{
    glClearColor(0.7f, 0.3f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO_FROM);
    glClearColor(0.07f, 0.13f, 0.27f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE,
                     glm::value_ptr(camera.viewMatrix));
    glm::vec4 po = glm::inverse(camera.viewMatrix)*glm::vec4(0,0,0,1); //w = 1bcs is a point
    glUniform4f(glGetUniformLocation(shaderProgram,"camPosition"),po.x,po.y,po.z,po.w);
    glUniform1i(glGetUniformLocation(shaderProgram,"normalATIVO"),normalATIVO);
    glUniform1f(glGetUniformLocation(shaderProgram,"normalV"),normalV);

}


void Render::renderDrawing(unsigned int layerIndex)
{
    MeshRenderData *layerData = renderData.layers + layerIndex;

    for(unsigned short i = 0; i < TEXTURE_HANDLERS_COUNT; i++) {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D_ARRAY,renderData.textureHandlers[i].texture);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_FROM);

    if(flags & MATRICES_CHANGE_FLAG) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, layerData->ssbos[SSBOS::ModelMatricesSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        16 * sizeof(float) * layerData->meshesCount,
                        layerData->matrices,GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    if(flags & LAMPS_CHANGE_FLAG) {
        glUniform1i(glGetUniformLocation(shaderProgram,"lampsCount"),renderData.lampsCount);
        flags = flags & ~(LAMPS_CHANGE_FLAG);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderData.lampsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(Lamp) * renderData.lampsCount,
                        renderData.lamps,GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    for(unsigned int i = 0; i < SSBO_PER_LAYER_COUNT; i++){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, layerData->ssbos[i]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, layerData->ssbos[i]);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindVertexArray(layerData->vao);
    glBindBuffer(GL_ARRAY_BUFFER, layerData->vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, layerData->vbos[1]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, layerData->ebo);


    if (transFeed && layerIndex == 1) {
        glBindBuffer(GL_ARRAY_BUFFER, TBO);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, TBO);
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, QUERY);
        glBeginTransformFeedback(GL_TRIANGLES);
    }

    glDrawElements(GL_TRIANGLES, layerData->verticesIndexCount, GL_UNSIGNED_INT,
                    0);


    if(transFeed && layerIndex == 1) {
        glEndTransformFeedback();
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

        glFlush();

        GLuint primitives;
        glGetQueryObjectuiv(QUERY, GL_QUERY_RESULT, &primitives);

        GLfloat *feedback = (GLfloat *)malloc(feedbacksize);
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbacksize, feedback);

        printf("%u vecs written!\n\n", primitives);

        for (unsigned int i = 0; i < feedbacknumber / 4; i++)
            printf("[%d] = %f %f %f %f\n", i,feedback[4 * i], feedback[4 * i + 1],
                    feedback[4 * i + 2],feedback[4 * i + 3]);
        printf("\n");
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}

void Render::start(void(*op1)(),void(*op2)(),void(*op3)()){

    logger.terminal.lines.push_back((char*)"viadinho de merda");

    char b[] = "/home/bandeira/Documents/GIT/PhysicsEngine/";
    assets.directory = (char*)malloc(sizeof(b));
    memcpy(assets.directory, b, sizeof(b));

    a.K[0] = glm::vec3(1,0,0);
    a.K[1] = glm::vec3(0,1,0);
    a.K[2] = glm::vec3(0,0,1);
    a.Ni = 0;
    a.bm = 0;
    a.d = 0;
    a.maps[MATERIAL_MAPS::KD]  = (char*)"assets/3dmodels/lemon_diff_4k.jpg";
    a.type = SOLID_COLOR;
    once();

    camera.translation[3][0] = 0;
    camera.translation[3][1] = 0;
    camera.translation[3][2] = 5;
    camera.rotation = glm::rotate(camera.rotation,3.14f, glm::vec3(0, 1, 0));


    renderData.allocLamp(LampsGenData{
        .position = glm::vec4(0,2,0,1),
    });
    a.type = TEXTURE;
    a.maps[MATERIAL_MAPS::KD] = (char*)"assets/3dmodels/Seta.png";
    renderData.pushModels(MeshGenData{
        (char*)"assets/3dmodels/Seta.obj"
    },a,LAYER::SPECIAL_LAYER);

    // a.maps[MATERIAL_MAPS::KD] = "assets/3dmodels/cannon_01_diff_4k.jpg";
    // a.maps[MATERIAL_MAPS::NORMAL] = "assets/3dmodels/cannon_01_nor_gl_4k.jpg";
    // renderData.pushModels(MeshGenData{
    //     "assets/3dmodels/cannon_01_4k.obj"
    // },a,LAYER::COMMON_LAYER);

    a.maps[MATERIAL_MAPS::KD] = (char*)"assets/3dmodels/gray_rocks_diff_4k.jpg";
    a.maps[MATERIAL_MAPS::NORMAL] = (char*)"assets/3dmodels/gray_rocks_nor_gl_4k.jpg";
    renderData.pushModels(MeshGenData{
        (char*)"assets/3dmodels/Plane.obj"
    },a,LAYER::COMMON_LAYER);


    a.K[0] = glm::vec3(1,1,1);
    a.K[1] = glm::vec3(0.8,0.8,0.8);
    a.K[2] = glm::vec3(0.5,0.5,0.5);

    a.maps[MATERIAL_MAPS::KD] = "assets/3dmodels/marble_bust_01_diff_4k.jpg";
    a.maps[MATERIAL_MAPS::NORMAL] = "assets/3dmodels/marble_bust_01_nor_gl_4k.jpg";
    renderData.pushModels(MeshGenData{
        "assets/3dmodels/marble_bust_01_4k.obj"
    },a,LAYER::COMMON_LAYER);



    MaterialGenData c;

    c.type = SOLID_COLOR;
    vectorMaterialI =  renderData.allocMaterial(c);
    assets.update();
    while(!glfwWindowShouldClose(glfwWin))
    {
        newframe();

        op1();

        update();

        op2();

        ui();

        op3();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glfwWin);
        glfwPollEvents();
        flags  = flags & ~(1&2);
    }
    free(assets.directory);
}


void Render::update() {
    if(flags & MATERIAL_CHANGE_FLAG) {
        flags = flags & ~(MATERIAL_CHANGE_FLAG);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderData.materialsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(Material) * renderData.materialsCount,
                        renderData.materials,GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }


    if(assets.framesDelay <= assets.delayCounter){
        assets.update();
        assets.delayCounter = 0;
    }

    for(int i = 0; i < LAYERS_COUNT; i++){
        if(renderData.delayCounter == 0 && flags & MODELS_CHANGE_FLAG) {
            updatePipeline(i);
        }
        renderDrawing(i);
    }

    if(renderData.delayCounter > 0)
            renderData.delayCounter--;
    flags = flags & ~(MODELS_CHANGE_FLAG);
    flags = flags & ~(MATRICES_CHANGE_FLAG);

    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER,FBO_FROM);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,FBO_TO);
        glBlitFramebuffer(0, 0, 1920, 1080,
                          0, 0, 1920, 1080,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    }
    assets.delayCounter++;
}

void Render::updatePipeline(unsigned int layerIndex){
    MeshRenderData *layerData = renderData.layers + layerIndex;
    if(renderData.delayCounter == -1)
        return;


    flags = flags | MATRICES_CHANGE_FLAG;
    // unsigned int  verticesCount = 0, verticesIndexCount = 0, textureVerticesCount=0,
    //                     textureVerticesIndexCount=0,normalVerticesCount=0,// normalIndexCount = nOfPrimitives = verticesIndexCount /3
    //                     verticesIndexOffset= 0,textureIndexOffset = 0,normalIndexOffset=0;
    // float *vertices,*matrices,*textureVertices,*normalVertices;
    // unsigned int *verticesIndex,*modelParent,*textureVerticesIndex,*normalVerticesIndex,*modelsMaterials;
    layerData->verticesCount = 0;
    layerData->verticesIndexCount = 0;
    layerData->textureVerticesCount = 0;
    layerData->textureVerticesIndexCount = 0;
    layerData->normalVerticesCount = 0;
    layerData->verticesIndexOffset = 0;
    if(renderData.delayCounter > 0){
            renderData.delayCounter--;
            return;
        }
    layerData->textureIndexOffset = 0;
    layerData->normalIndexOffset = 0;
    unsigned int nModels = layerData->models.size();
    for(unsigned int i = 0; i < nModels; i++){
        Model actualModel = layerData->models[i];
        layerData->verticesCount += actualModel.mesh.verticesCount;
        layerData->verticesIndexCount += actualModel.mesh.verticesIndexCount;
        layerData->textureVerticesCount += actualModel.mesh.textureVerticesCount;
        layerData->textureVerticesIndexCount += actualModel.mesh.textureVerticesIndexCount;
        layerData->normalVerticesCount += actualModel.mesh.normalVerticesCount;
    }

    layerData->meshesCount = nModels;

    layerData->vertices = (float*)realloc(layerData->vertices,
                        sizeof(float) * layerData->verticesCount);
    layerData->matrices = (float*)realloc(layerData->matrices,
                        sizeof(float) * 16 * layerData->meshesCount);
    layerData->textureVertices = (float*)realloc(layerData->textureVertices,
                        sizeof(float) * layerData->textureVerticesCount);
    layerData->normalVertices = (float*)realloc(layerData->normalVertices,
                        sizeof(float) * layerData->normalVerticesCount);

    layerData->verticesIndex = (unsigned int*)realloc(layerData->verticesIndex,
                        sizeof(unsigned int) * layerData->verticesIndexCount);
    layerData->modelParent = (unsigned int*)realloc(layerData->modelParent,
                        sizeof(unsigned int) * layerData->verticesCount / 3);
    layerData->textureVerticesIndex = (unsigned int*)realloc(layerData->textureVerticesIndex,
                        sizeof(unsigned int) * layerData->textureVerticesIndexCount);
    layerData->normalVerticesIndex = (unsigned int*)realloc(layerData->normalVerticesIndex,
                        sizeof(unsigned int) * layerData->verticesIndexCount / 3);
    layerData->modelMaterial = (unsigned int*)realloc(layerData->modelMaterial,
                        sizeof(unsigned int) * layerData->meshesCount);


    unsigned int localVerticesNumber = 0;
    unsigned int localVerticesIndexNumber = 0;
    unsigned int localTextureVerticesNumber = 0;
    unsigned int localTextureVerticesIndexNumber = 0;
    unsigned int localNormalVerticesNumber = 0;

    for(unsigned int i = 0; i < nModels; i++){
        Mesh actualMesh = layerData->models[i].mesh;
        memcpy(layerData->vertices + localVerticesNumber,
            actualMesh.vertices, sizeof(float) * actualMesh.verticesCount);
        memcpy(layerData->matrices + 16 *  i,
            glm::value_ptr(layerData->models[i].matrix), sizeof(float) * 16);
        memcpy(layerData->textureVertices + localTextureVerticesNumber,
            actualMesh.textureVertices, sizeof(float) * actualMesh.textureVerticesCount);
        memcpy(layerData->normalVertices + localNormalVerticesNumber,
            actualMesh.normalVertices, sizeof(float) * actualMesh.normalVerticesCount);



        memcpy(layerData->verticesIndex + localVerticesIndexNumber,
            actualMesh.verticesIndex, sizeof(unsigned int) * actualMesh.verticesIndexCount);
        memcpy(layerData->textureVerticesIndex + localTextureVerticesIndexNumber,
            actualMesh.textureVerticesIndex, sizeof(unsigned int) * actualMesh.textureVerticesIndexCount);
        memcpy(layerData->normalVerticesIndex + localVerticesIndexNumber / 3,
            actualMesh.normalVerticesIndex, sizeof(unsigned int) * actualMesh.verticesIndexCount / 3);


        layerData->modelMaterial[i] = layerData->models[i].materialIndex;

        for(unsigned int j = 0; j < actualMesh.verticesCount / 3; j++){
            layerData->modelParent[j + localVerticesNumber / 3] = i;
        }

        for(unsigned int j = 0; j < actualMesh.verticesIndexCount; j++){
            *(layerData->verticesIndex + localVerticesIndexNumber + j) += localVerticesNumber/3;
        }
        for(unsigned int j = 0; j < actualMesh.textureVerticesIndexCount; j++){
            *(layerData->textureVerticesIndex + localVerticesIndexNumber + j) += localTextureVerticesNumber/2;
        }
        for(unsigned int j = 0; j < actualMesh.verticesIndexCount / 3; j++){
            *(layerData->normalVerticesIndex + localVerticesIndexNumber/3 + j) += localNormalVerticesNumber/3;
        }


        localVerticesNumber += actualMesh.verticesCount;
        localVerticesIndexNumber += actualMesh.verticesIndexCount;
        localTextureVerticesNumber += actualMesh.textureVerticesCount;
        localTextureVerticesIndexNumber += actualMesh.textureVerticesIndexCount;
        localNormalVerticesNumber += actualMesh.normalVerticesCount;
    }

    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,layerData->ssbos[SSBOS::NormalVecsIndexSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(unsigned int) * layerData->verticesIndexCount/3,
                        layerData->normalVerticesIndex,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,layerData->ssbos[SSBOS::NormalVecsSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(float) * layerData->normalVerticesCount,
                        layerData->normalVertices,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,layerData->ssbos[SSBOS::TextureIndexSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(unsigned int) * layerData->textureVerticesIndexCount,
                        layerData->textureVerticesIndex,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,layerData->ssbos[SSBOS::TextureCoordSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(float) * layerData->textureVerticesCount,
                        layerData->textureVertices,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,layerData->ssbos[SSBOS::ModelsMaterialsSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(unsigned int) * layerData->meshesCount,
                        layerData->modelMaterial,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, layerData->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        sizeof(unsigned int) * layerData->verticesIndexCount,
                        layerData->verticesIndex,GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(layerData->vao);
        glBindBuffer(GL_ARRAY_BUFFER,layerData->vbos[0]);
        glBufferData(GL_ARRAY_BUFFER,  sizeof(float) * layerData->verticesCount,
                            layerData->vertices,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER,layerData->vbos[1]);
        glBufferData(GL_ARRAY_BUFFER,  sizeof(int) * layerData->verticesCount/3,
                            layerData->modelParent,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }

}
