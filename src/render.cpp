
#include "render.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "utils.hpp"
#include "vertexArray.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <stdlib.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "stb_image/stb_image.h"


#define VERTEX_SHADERS_LOCALPATH "vShader.vert"
#define FRAGMENT_SHADERS_LOCALPATH "fShader.frag"
#define GEOMETRY_SHADERS_LOCALPATH "gShader.geom"
#define LEVEL 0

int fodase[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

RenderData::RenderData() : models()
{

    vertices = (float *)malloc(0);
    matrices = (float *)malloc(0);
    textureVertices = (float *)malloc(0);
    normalVec = (float *)malloc(0);


    verticesIndex = (unsigned int *)malloc(0);
    matricesIndex = (unsigned int *)malloc(0);
    textureVerticesIndex = (unsigned int *)malloc(0);
    normalIndex = (unsigned int *)malloc(0);
    textureIndxDATA = (unsigned int *)malloc(0);
    for(int i =0; i < TEXTURE_COUNT; i++){
        modelsPerTex[i] = (unsigned short *)malloc(0);
    }
}


void RenderData::freeRenderData()
{
    for(int i = 0; i <models.size(); i++){
        free(models[i].mesh.vertices);
        free(models[i].mesh.textureVertices);
        free(models[i].mesh.normalVec);
        free(models[i].mesh.verticesIndex);
        free(models[i].mesh.textureVerticesIndex);
        free(models[i].mesh.normalIndex);
    }
    free(textureIndxDATA);

    free(vertices);
    free(matrices);
    free(textureVertices);
    free(normalVec);

    free(verticesIndex);
    free(matricesIndex);
    free(textureVerticesIndex);
    free(normalIndex);
}

Camera::Camera(glm::mat4 vMatrix,glm::mat4 pjMatrix):viewMatrix(vMatrix),
                    projMatrix(pjMatrix),renderData(){

}

glm::mat4 *Camera::getNMatrix(unsigned short index){
    return (glm::mat4*)(renderData.matrices + 16*index);
}

// 0123456789
// x01x23x456
unsigned short getOldIndexOfNew(unsigned short i, unsigned short n,unsigned short *index){
    unsigned short rt = i;
    for (unsigned short j = 0; j < n; j++){
        if(index[j] <= rt){
            rt++;
        }
    }
    return rt;
}


unsigned short getNewIndexOfOld(unsigned short i, unsigned short n,unsigned short *index){
    unsigned short rt = i;
    for (unsigned short j = 0; j < n; j++){
        if(index[j] == i){
            sendError("Acess of Delete Index");
            return 0;
        }
        if(index[j] < i)
            rt--;
    }
    return  rt;
}




Render::Render(glm::mat4 vMatrix, glm::mat4 pjMatrix,GLFWwindow *win)
    : camera(vMatrix,pjMatrix),
      m_VAO(std::make_unique<VAO>()),
      shader((const char *[3]){VERTEX_SHADERS_LOCALPATH,
                               FRAGMENT_SHADERS_LOCALPATH,
                               GEOMETRY_SHADERS_LOCALPATH}),glfwWin(win){
    shaderProgram = glCreateProgram();
    shader.attach(shaderProgram);

    if (transFeed) {
        const GLchar *feedbackVaryings[] = {"outValue"};
        glTransformFeedbackVaryings(shaderProgram, 1, feedbackVaryings,
                                    GL_INTERLEAVED_ATTRIBS);

        // feedbacknumber = 4*model.mesh.verticesIndexCount;
        feedbacksize = feedbacknumber * sizeof(float);
    }

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
}

Render::~Render() {

    camera.renderData.freeRenderData();
    if (transFeed) {
        glDeleteBuffers(1, &TBO);
    }
    m_VAO.reset();
    glDeleteProgram(shaderProgram);
    glDeleteFramebuffers(1, &FBO_FROM);
    glDeleteRenderbuffers(1, &RBO);
    // glDeleteTextures(1,&renderTex);
    glDeleteQueries(1, &Query);
    glDeleteBuffers(1, &EBO);
}


void Render::input()
{
    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            glfwSetWindowShouldClose(glfwWin, true);
        }
    if (ImGui::IsWindowFocused())
    {
        if(camera.renderData.models.size() > camera.selectedModelIndex){
            glm::mat4 *modelMatrix = camera.getNMatrix(camera.selectedModelIndex);
            if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
            {
                *modelMatrix =
                    glm::rotate(*modelMatrix, glm::radians(-10.0f), glm::vec3(1, 0, 0));
                flags = flags | 4;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
            {
                *modelMatrix =
                    glm::rotate(*modelMatrix, glm::radians(2.0f), glm::vec3(1, 0, 0));
                flags = flags | 4;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
            {
                *modelMatrix =
                    glm::rotate(*modelMatrix, glm::radians(-10.0f), glm::vec3(0, 1, 0));
                flags = flags | 4;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
            {
                *modelMatrix =
                    glm::rotate(*modelMatrix, glm::radians(2.0f), glm::vec3(0, 1, 0));
                flags = flags | 4;

            }
            if (ImGui::IsKeyPressed(ImGuiKey_A))
            {
                *modelMatrix = glm::translate(*modelMatrix, glm::vec3(1, 0, 0));
                flags = flags | 4;

                // camera.viewMatrix[3][2] -= .2;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_D))
            {
                *modelMatrix = glm::translate(*modelMatrix, glm::vec3(-1, 0, 0));
                flags = flags | 4;


                // camera.viewMatrix[3][2] += .2;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_W))
            {
                *modelMatrix = glm::translate(*modelMatrix, glm::vec3(0, 0, 1));
                flags = flags | 4;


                // camera.viewMatrix[3][0] -= .2;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_S))
            {
                *modelMatrix = glm::translate(*modelMatrix, glm::vec3(0, 0, -1));
                flags = flags | 4;

                // camera.viewMatrix[3][0] += .2;
            }
        }
    }
}



void Render::once()
{
    if (transFeed) {
        glGenQueries(1, &Query);
        glGenBuffers(1, &TBO);
        glBindBuffer(GL_ARRAY_BUFFER, TBO);
        glBufferData(GL_ARRAY_BUFFER, feedbacksize, nullptr, GL_STATIC_READ);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    m_VAO->bind();
    m_VAO->createVBO();
    m_VAO->bindVBO(0);

    // VBO Binded
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * camera.renderData.verticesCount,
                camera.renderData.vertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *)0);

    m_VAO->createVBO();
    m_VAO->bindVBO(1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(int) * camera.renderData.verticesCount/3,
                camera.renderData.matricesIndex, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(int),
                            (void *)0); // set vec3 positi

    m_VAO->unbindVBO();
    m_VAO->unbind();

    // ModelMatricesSSBO
    glGenBuffers(1, &modelMxSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMxSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMxSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    // TextureCoordSSBO
    glGenBuffers(1, &texCoordSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, texCoordSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, texCoordSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    // TextureIndexSSBO
    glGenBuffers(1, &texIndexSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, texIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, texIndexSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    // EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                        GL_FALSE, glm::value_ptr(camera.projMatrix));


    //------//------//------//------//
    // FBO
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

    //------//------//------//------//------//


    glGenFramebuffers(1, &FBO_TO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_TO);

    //texToShow
    glGenTextures(1, &texToShowFrom);
    glBindTexture(GL_TEXTURE_2D, texToShowFrom);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB,
                GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            texToShowFrom, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    for(int i = 0; i < TEXTURE_COUNT; i++){
        camera.renderData.texturesCount[i] = 0;
    }

    glGenTextures(TEXTURE_COUNT,texARRAY);
    for(unsigned short i = 0; i < TEXTURE_COUNT; i++){
        unsigned short k = pow(2,6+i);
        glBindTexture(GL_TEXTURE_2D_ARRAY,texARRAY[i]);
        glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,k,k,0,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, LEVEL);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    // textureIndxSSBO
    // for(int i = 0; i < camera.renderData.models.size(); i++){
    //     printf("vaia%d %d\n",camera.renderData.textureIndxDATA[2*i],camera.renderData.textureIndxDATA[2*i+1]);
    // }
    glGenBuffers(1, &textureIndxSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureIndxSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, textureIndxSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind


    glGenTextures(1, &texUtilitary);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texUtilitary);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,0,0,0,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, LEVEL);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glUniform1iv(glGetUniformLocation(shaderProgram, "textures"), TEXTURE_COUNT,fodase);


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
}


void Render::renderDrawing()
{
    // model.tex.Bind(1);
    if(flags & 4){
        flags = flags & ~(4);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMxSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        16 * sizeof(float) * camera.renderData.models.size(),
                        camera.renderData.matrices,GL_DYNAMIC_DRAW);
    }

    if(flags & 1 || flags & 2){
        m_VAO->bind();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        sizeof(unsigned int) * camera.renderData.verticesIndexCount,
                        camera.renderData.verticesIndex,GL_DYNAMIC_DRAW);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //Repassing ModelsMatricesSSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMxSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        16 * sizeof(float) * camera.renderData.models.size(),
                        camera.renderData.matrices,GL_DYNAMIC_DRAW);

        // TextureCoordSSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, texCoordSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                    sizeof(float) * camera.renderData.textureVerticesCount,
                    camera.renderData.textureVertices, GL_DYNAMIC_DRAW);

        // TextureIndexSSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, texIndexSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                    sizeof(int) * camera.renderData.textureVerticesIndexCount,
                    camera.renderData.textureVerticesIndex, GL_DYNAMIC_DRAW);

        // printf("\n");
        // for(int i = 0; i < camera.renderData.models.size(); i++){
        //     printf("pinta%d %d\n",camera.renderData.textureIndxDATA[2*i],camera.renderData.textureIndxDATA[2*i+1]);
        // }
        // printf("\n");
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureIndxSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                    sizeof(int) * 2 * camera.renderData.models.size(),
                    camera.renderData.textureIndxDATA, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind


        m_VAO->bindVBO(1);
        glBufferData(GL_ARRAY_BUFFER,  sizeof(int) * camera.renderData.verticesCount/3,
                            camera.renderData.matricesIndex,GL_DYNAMIC_DRAW);
        m_VAO->bindVBO(0);
        glBufferData(GL_ARRAY_BUFFER,  sizeof(float) * camera.renderData.verticesCount,
                        camera.renderData.vertices,GL_DYNAMIC_DRAW);


        for(unsigned short i = 0; i < TEXTURE_COUNT; i++){
            glActiveTexture(GL_TEXTURE0+i);
            glBindTexture(GL_TEXTURE_2D_ARRAY,texARRAY[i]);
        }
    }
    else {
        m_VAO->bind();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMxSSBO);

        // TextureCoordSSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, texCoordSSBO);

        // TextureIndexSSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, texIndexSSBO);

        m_VAO->bindVBO(1);

        m_VAO->bindVBO(0);

        for(unsigned short i = 0; i < TEXTURE_COUNT; i++){
            glActiveTexture(GL_TEXTURE0+i);
            glBindTexture(GL_TEXTURE_2D_ARRAY,texARRAY[i]);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_FROM);
    glClearColor(0.7f, 0.3f, 0.17f, 1.0f);

    if (transFeed) {
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, TBO);
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, Query);
        glBeginTransformFeedback(GL_TRIANGLES);
    }

    glDrawElements(GL_TRIANGLES, camera.renderData.verticesIndexCount, GL_UNSIGNED_INT,
                    0);

    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // TextureCoordSSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // TextureIndexSSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_VAO->unbindVBO();

    m_VAO->unbind();
    // model.tex.Unbind();

    if (transFeed)
    {
        glEndTransformFeedback();
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

        glFlush();

        GLuint primitives;
        glGetQueryObjectuiv(Query, GL_QUERY_RESULT, &primitives);

        GLfloat *feedback = (GLfloat *)malloc(feedbacksize);
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbacksize, feedback);

        printf("%u vecs written!\n\n", primitives);

        for (int i = 0; i < feedbacknumber / 4; i++) {
        printf("%f %f %f\n ", feedback[3 * i], feedback[3 * i + 1],
                feedback[3 * i + 2]);
        }
        printf("\n\n");
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
    }
}



void Render::imguiSetting()
{
    ImGui::NewFrame();
    if(ImGui::BeginMainMenuBar()){
        if(ImGui::BeginMenu("File")){
            ImGui::MenuItem("Pinto");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if(ImGui::Begin("Assets")){
        ImGui::End();
    }
    if(ImGui::Begin("Models")){
        if(ImGui::Button("Add")){
            if(camera.selectedModelIndex < camera.renderData.models.size()){
                ModelGenStruct m;
                m.meshPath = camera.renderData.models[camera.selectedModelIndex].mesh.meshPath;
                m.texPath  = camera.renderData.models[camera.selectedModelIndex].tex.localPath;
                addModels(1, &m);
            }
        }
        if(ImGui::Button("Remove")){
            if(camera.renderData.models.size() > camera.selectedModelIndex){
                printf("aa%d\n", camera.selectedModelIndex);
                rmModels(1, &camera.selectedModelIndex);
                camera.selectedModelIndex--;
            }

        }
        if(ImGui::BeginTable("Modelss1", 1)){
            char label[100];
            for(int i = 0; i < camera.renderData.models.size(); i++){
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                sprintf(label, "Model %d",i);
                if(i == camera.selectedModelIndex){
                    ImGui::TableSetBgColor(
                        ImGuiTableBgTarget_RowBg0,
                        IM_COL32(38, 38, 38, 255)
                    );
                }
                if(ImGui::Selectable(label)){
                    camera.selectedModelIndex = i;
                }
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }



    ImGui::Begin("Render");
    input();
    ImGui::GetWindowDrawList()->AddImage((void *)texToShowFrom, ImGui::GetWindowPos(),
                                        ImGui::GetWindowSize() +
                                            ImGui::GetWindowPos(),
                                        ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    bool a = 1;
    ImGui::ShowDemoWindow(&a);
}


void Render::start(void(*op1)(),void(*op2)(),void(*op3)()){
    once();
    ModelGenStruct models[3] =
    {
        ModelGenStruct
        {
            meshPath:"assets/3dmodels/Cube.obj",
            texPath:"assets/3dmodels/CubeTexture2.jpg"
        },
        ModelGenStruct
        {
            meshPath:"assets/3dmodels/cannon_01_4k.obj",
            texPath:"assets/3dmodels/cannon_01_diff_4k.jpg"
        },
        ModelGenStruct
        {
            meshPath:"assets/3dmodels/marble_bust_01_4k.obj",
            texPath:"assets/3dmodels/marble_bust_01_diff_4k.jpg"
        }

    };

    addModels(3, models);
    // addModels(1, models);
    // addModels(1, &models[1]);

    // addModels(1, &models[1]);
    unsigned short  a = 0;
    rmModels(1, &a);
    while(!glfwWindowShouldClose(glfwWin))
    {
        newframe();

        op1();

        renderDrawing();

        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER,FBO_FROM);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER,FBO_TO);
            glBlitFramebuffer(0, 0, 1920, 1080,
                              0, 0, 1920, 1080,
                              GL_COLOR_BUFFER_BIT, GL_LINEAR);
            glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
        }

        op2();

        imguiSetting();//Here is where im removing


        op3();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glfwWin);
        glfwPollEvents();
        flags  = flags & ~(1&2);
    }
}



void Render::addModels(unsigned short n, ModelGenStruct *data){
    flags = flags | 1; // set Add Flag

    unsigned int totalVerticesCount = camera.renderData.verticesCount,
                 totalTextureVerticesCount = camera.renderData.textureVerticesCount,
                 totalNormalVecCount = camera.renderData.normalVecCount,
                 totalVerticesIndexCount = camera.renderData.verticesIndexCount,
                 totalTextureVerticesIndexCount = camera.renderData.textureVerticesIndexCount,
                 totalNormalIndexCount = camera.renderData.normalIndexCount;


    for (unsigned short i = 0; i < n; i++){
        camera.renderData.models.push_back(Model(data[i].meshPath,data[i].texPath));
        Model &actualM = camera.renderData.models.back();
        totalVerticesCount += actualM.mesh.verticesCount;
        totalTextureVerticesCount += actualM.mesh.textureVerticesCount;
        totalNormalVecCount += actualM.mesh.normalVecCount;
        totalVerticesIndexCount += actualM.mesh.verticesIndexCount;
        totalTextureVerticesIndexCount += actualM.mesh.textureVerticesIndexCount;
        totalNormalIndexCount += actualM.mesh.normalIndexCount;
    }

    unsigned long nModels = camera.renderData.models.size();
    camera.renderData.vertices = (float *)realloc(camera.renderData.vertices,
                sizeof(float) * totalVerticesCount);
    camera.renderData.matrices = (float *)realloc(camera.renderData.matrices,
                sizeof(float) * 16 * nModels);
    camera.renderData.textureVertices = (float *)realloc(camera.renderData.textureVertices,
                sizeof(float) * totalTextureVerticesCount);
    camera.renderData.normalVec = (float *)realloc(camera.renderData.normalVec,
                sizeof(float) * totalNormalVecCount);

    camera.renderData.verticesIndex = (unsigned int *)realloc(camera.renderData.verticesIndex,
                sizeof(int) * totalVerticesIndexCount);
    camera.renderData.matricesIndex = (unsigned int *)realloc(camera.renderData.matricesIndex,
                sizeof(int) * totalVerticesCount/3);
    camera.renderData.textureVerticesIndex = (unsigned int *)realloc(camera.renderData.textureVerticesIndex,
                sizeof(int) * totalTextureVerticesIndexCount);
    camera.renderData.normalIndex = (unsigned int *)realloc(camera.renderData.normalIndex,
                sizeof(int) * totalNormalIndexCount);


    camera.renderData.textureIndxDATA = (unsigned int*)realloc(camera.renderData.textureIndxDATA, sizeof(int) * 2 * nModels);


    for (unsigned short i = 0; i < n; i++){

        Model &actualM = camera.renderData.models[nModels - n + i];


    //-------/-------/-------/----- TEXTURE -----/-------/-------/-------/-------/

        stbi_set_flip_vertically_on_load (1);
        unsigned char *localBuffer = stbi_load(actualM.tex.localPath, &actualM.tex.width,  &actualM.tex.height, &actualM.tex.BPP, 4);
        if(actualM.tex.width != actualM.tex.height){
            sendError("Width != Height");
            exit(EXIT_FAILURE);
        }
        if(actualM.tex.width > 4096){
            sendError("Width,Height > 4096");
            exit(EXIT_FAILURE);
        }
        unsigned char buffIndex = (unsigned char)floor(log2(actualM.tex.height)-6);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texUtilitary);
        glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,actualM.tex.width,actualM.tex.width,camera.renderData.texturesCount[buffIndex]+1,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, LEVEL);

        glCopyImageSubData(texARRAY[buffIndex],GL_TEXTURE_2D_ARRAY,LEVEL,0,0,0,texUtilitary,GL_TEXTURE_2D_ARRAY,LEVEL,
                        0,0,0,actualM.tex.width,actualM.tex.width,camera.renderData.texturesCount[buffIndex]);

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, LEVEL, 0, 0,
                            camera.renderData.texturesCount[buffIndex], actualM.tex.width, actualM.tex.width, 1, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);

        glBindTexture(GL_TEXTURE_2D_ARRAY,texARRAY[buffIndex]);

        glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,0,0,0,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
        glBindTexture(GL_TEXTURE_2D_ARRAY,0);



        actualM.tex.position = glm::vec2(buffIndex,camera.renderData.texturesCount[buffIndex]);
        camera.renderData.textureIndxDATA[2*(nModels - n + i)] = buffIndex;
        camera.renderData.textureIndxDATA[2*(nModels - n + i)+1] = camera.renderData.texturesCount[buffIndex];
        camera.renderData.texturesCount[buffIndex]++;
        camera.renderData.modelsPerTex[buffIndex] = (unsigned short*)realloc(camera.renderData.modelsPerTex[buffIndex], camera.renderData.texturesCount[buffIndex]*sizeof(short));
        camera.renderData.modelsPerTex[buffIndex][camera.renderData.texturesCount[buffIndex]-1] =(nModels - n + i);
        stbi_image_free (localBuffer);

        unsigned int cache = texARRAY[buffIndex];
        texARRAY[buffIndex] = texUtilitary;
        texUtilitary = cache;


    //-------/-------/-------/----- FLOAT* -----/-------/-------/-------/-------/

        memcpy(camera.renderData.vertices + camera.renderData.verticesCount,
                actualM.mesh.vertices, sizeof(float) * actualM.mesh.verticesCount);
        camera.renderData.verticesCount += actualM.mesh.verticesCount;


        memcpy(camera.renderData.matrices + 16 * (nModels - n + i),
                glm::value_ptr(actualM.mesh.matrix), 16 * sizeof(float));


        memcpy(camera.renderData.textureVertices + camera.renderData.textureVerticesCount,
                actualM.mesh.textureVertices, sizeof(float) * actualM.mesh.textureVerticesCount);
        camera.renderData.textureVerticesCount += actualM.mesh.textureVerticesCount;



        memcpy(camera.renderData.normalVec + camera.renderData.normalVecCount,
                actualM.mesh.normalVec, sizeof(float) * actualM.mesh.normalVecCount);
        camera.renderData.normalVecCount += actualM.mesh.normalVecCount;


    //-------/-------/-------/----- UINT* -----/-------/-------/-------/-------/



        memcpy(camera.renderData.verticesIndex + camera.renderData.verticesIndexCount,
                actualM.mesh.verticesIndex,
                sizeof(int) * actualM.mesh.verticesIndexCount);



        unsigned int *p =camera.renderData.matricesIndex+(camera.renderData.verticesCount- actualM.mesh.verticesCount)/3;
        for (int j = 0; j < actualM.mesh.verticesCount/3; j++) {
            p[j] = nModels - n + i;
        }


        memcpy(camera.renderData.textureVerticesIndex + camera.renderData.textureVerticesIndexCount,
                actualM.mesh.textureVerticesIndex,
                sizeof(int) * actualM.mesh.textureVerticesIndexCount);

        memcpy(camera.renderData.normalIndex + camera.renderData.normalIndexCount,
                actualM.mesh.normalIndex,
                sizeof(int) * actualM.mesh.normalIndexCount);

        for(unsigned int j = 0; j < actualM.mesh.verticesIndexCount; j++){
            *(camera.renderData.verticesIndex + camera.renderData.verticesIndexCount + j) += camera.renderData.verticesIndexOffset;
        }
        for(unsigned int j = 0; j < actualM.mesh.textureVerticesIndexCount; j++){
            *(camera.renderData.textureVerticesIndex + camera.renderData.textureVerticesIndexCount + j) += camera.renderData.textureIndexOffset;
        }
        for(unsigned int j = 0; j < actualM.mesh.normalIndexCount; j++){
            *(camera.renderData.normalIndex + camera.renderData.normalIndexCount + j) += camera.renderData.normalIndexOffset;
        }
        camera.renderData.verticesIndexCount += actualM.mesh.verticesIndexCount;
        camera.renderData.textureVerticesIndexCount += actualM.mesh.textureVerticesIndexCount;
        camera.renderData.normalIndexCount += actualM.mesh.normalIndexCount;

        camera.renderData.verticesIndexOffset += actualM.mesh.verticesCount/3;
        camera.renderData.textureIndexOffset += actualM.mesh.textureVerticesCount/2;
        camera.renderData.normalIndexOffset += actualM.mesh.normalVecCount/3;


    }
}



void Render::rmModels(unsigned short n, unsigned short *indices){
    flags = flags | 2; // set Rm Flag
    float *antiqueMatrices = (float *)malloc(sizeof(float) * 16 *camera.renderData.models.size());
    memcpy(antiqueMatrices, camera.renderData.matrices, sizeof(float) * 16 *camera.renderData.models.size());
    unsigned short antiqueTexturesCount[TEXTURE_COUNT];
    // memcpy(antiqueTextureIndxDATA, camera.renderData.textureIndxDATA, sizeof(int) * 2 * camera.renderData.models.size());
    // unsigned short *texPerModel = (unsigned short *)malloc(sizeof(short) * camera.renderData.models.size());
    // for(unsigned short i = 0; i < camera.renderData.models.size(); i++){
    //     antiqueTextureIndxDATA[2*i];
    // }
    // sort(indices);
    unsigned short indicesPerTex[TEXTURE_COUNT];
    unsigned short *rmPerTex[TEXTURE_COUNT];

    for(unsigned short i = 0; i < TEXTURE_COUNT; i++){
        antiqueTexturesCount[i] = camera.renderData.texturesCount[i];
        indicesPerTex[i] = 0;
        rmPerTex[i] = (unsigned short*)malloc(sizeof(short) * n);
    }

    for(unsigned short i = 0; i < n; i++){
        unsigned short kk = camera.renderData.models[indices[i]].tex.position.x;
        rmPerTex[kk][indicesPerTex[kk]] = camera.renderData.models[indices[i]].tex.position.y;
        indicesPerTex[kk]++;
        camera.renderData.texturesCount[kk]--;
    }

    for(unsigned short i = 0; i < TEXTURE_COUNT; i++){
        if(indicesPerTex[i]){
            unsigned short initOfCopy = 0;
            unsigned short toPaste = 0;
            unsigned short *koooo = camera.renderData.modelsPerTex[i];
            float size = pow(2, i + 6);
            unsigned short sooofunckingIndices = 0;
            for(unsigned short j = 0; j < indicesPerTex[i]; j++){
                // if(initOfCopy == rmPerTex[i][j])
                //     initOfCopy++;

                unsigned short nToCopy = rmPerTex[i][j] - initOfCopy;
                if(nToCopy < 1){
                    initOfCopy = rmPerTex[i][j] + 1;
                    continue;
                }


                glCopyImageSubData(texARRAY[i], GL_TEXTURE_2D_ARRAY, LEVEL, 0, 0, initOfCopy,
                                   texARRAY[i], GL_TEXTURE_2D_ARRAY, LEVEL, 0, 0, toPaste, size, size, nToCopy);
                for(unsigned short k = 0; k < nToCopy; k++){
                    camera.renderData.models[koooo[initOfCopy+k]].tex.position.y = sooofunckingIndices;
                    sooofunckingIndices++;
                }
                initOfCopy = rmPerTex[i][j] + 1;
                toPaste += nToCopy;
            }


            unsigned short nToCopy = antiqueTexturesCount[i] - initOfCopy;
            if(nToCopy > 0)
                glCopyImageSubData(texARRAY[i],GL_TEXTURE_2D_ARRAY,LEVEL,0,0,initOfCopy,texARRAY[i],GL_TEXTURE_2D_ARRAY,LEVEL,0,0,toPaste,size,size,nToCopy);


            glBindTexture(GL_TEXTURE_2D_ARRAY, texUtilitary);
            glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,size,size,camera.renderData.texturesCount[i],0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, LEVEL);

            glCopyImageSubData(texARRAY[i], GL_TEXTURE_2D_ARRAY, LEVEL, 0, 0, 0, texUtilitary, GL_TEXTURE_2D_ARRAY, LEVEL,
                               0, 0, 0, size, size, camera.renderData.texturesCount[i]);

            glBindTexture(GL_TEXTURE_2D_ARRAY,texARRAY[i]);

            glTexImage3D(GL_TEXTURE_2D_ARRAY, LEVEL, GL_RGBA8, 0, 0, 0, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glBindTexture(GL_TEXTURE_2D_ARRAY,0);

            unsigned int cache = texARRAY[i];
            texARRAY[i] = texUtilitary;
            texUtilitary = cache;
        }
    }


    for (unsigned short i = 0; i < n; i++)
    {
        // indexOfRemove = indexOfRemove > index[i] ? index[i] : indexOfRemove;
        Model actualM = camera.renderData.models.at(indices[i]);

        camera.renderData.models.erase(camera.renderData.models.begin() + indices[i]);
        camera.renderData.verticesCount -= actualM.mesh.verticesCount;
        camera.renderData.verticesIndexCount -= actualM.mesh.verticesIndexCount;
        camera.renderData.textureVerticesCount  -= actualM.mesh.textureVerticesCount;
        camera.renderData.textureVerticesIndexCount -= actualM.mesh.textureVerticesIndexCount;
        camera.renderData.normalVecCount -= actualM.mesh.normalVecCount;
        camera.renderData.normalIndexCount -= actualM.mesh.normalIndexCount;
    }


    camera.renderData.vertices = (float *)realloc(camera.renderData.vertices,
                sizeof(float) * camera.renderData.verticesCount);
    camera.renderData.matrices = (float *)realloc(camera.renderData.matrices,
                sizeof(float) * 16 * camera.renderData.models.size());
    camera.renderData.textureVertices = (float *)realloc(camera.renderData.textureVertices,
                sizeof(float) * camera.renderData.textureVerticesCount);
    camera.renderData.normalVec = (float *)realloc(camera.renderData.normalVec,
                sizeof(float) *  camera.renderData.normalVecCount);

    camera.renderData.verticesIndex = (unsigned int *)realloc(camera.renderData.verticesIndex,
                sizeof(int) * camera.renderData.verticesIndexCount);
    camera.renderData.matricesIndex = (unsigned int *)realloc(camera.renderData.matricesIndex,
                sizeof(int) * camera.renderData.verticesCount/3);
    camera.renderData.textureVerticesIndex = (unsigned int *)realloc(camera.renderData.textureVerticesIndex,
                sizeof(int) * camera.renderData.textureVerticesIndexCount);
    camera.renderData.normalIndex = (unsigned int *)realloc(camera.renderData.normalIndex,
                sizeof(int) * camera.renderData.normalIndexCount);




    camera.renderData.verticesIndexOffset = 0;
    camera.renderData.textureIndexOffset = 0;
    camera.renderData.normalIndexOffset = 0;
    unsigned int indexVerticesCount = 0,indexVerticesIndexCount = 0,indexTextureVerticesCount = 0,
                 indexTextureVerticesIndexCount = 0,indexNormalVecCount = 0,indexNormalIndexCount = 0,indexMatricesIndexCount = 0;


    for(unsigned int i = 0; i < camera.renderData.models.size(); i++){
        Model actualM = camera.renderData.models[i];

        camera.renderData.modelsPerTex[actualM.tex.position.x] = (unsigned short*)realloc(camera.renderData.modelsPerTex[actualM.tex.position.x],
                                                                        camera.renderData.texturesCount[actualM.tex.position.x]*sizeof(short));
        camera.renderData.modelsPerTex[actualM.tex.position.x][camera.renderData.texturesCount[actualM.tex.position.x]-1] = i;
        camera.renderData.textureIndxDATA[2*i] = actualM.tex.position.x;
        camera.renderData.textureIndxDATA[2*i+1] = actualM.tex.position.y;

        //-------/-------/-------/----- FLOAT* -----/-------/-------/-------/-------/
        memcpy(camera.renderData.vertices + indexVerticesCount,
                actualM.mesh.vertices, sizeof(float) * actualM.mesh.verticesCount);
        indexVerticesCount +=actualM.mesh.verticesCount;

        memcpy(camera.renderData.matrices + 16 * i,
                 antiqueMatrices + 16*getOldIndexOfNew(i, n, indices), 16 * sizeof(float));

        memcpy(camera.renderData.textureVertices + indexTextureVerticesCount,
                actualM.mesh.textureVertices, sizeof(float) * actualM.mesh.textureVerticesCount);
        indexTextureVerticesCount +=actualM.mesh.textureVerticesCount;

        memcpy(camera.renderData.normalVec + indexNormalVecCount,
                actualM.mesh.normalVec, sizeof(float) * actualM.mesh.normalVecCount);
        indexNormalVecCount +=actualM.mesh.normalVecCount;

        //-------/-------/-------/----- UINT* -----/-------/-------/-------/-------/
        memcpy(camera.renderData.verticesIndex + indexVerticesIndexCount,
                actualM.mesh.verticesIndex,
                sizeof(int) * actualM.mesh.verticesIndexCount);

        unsigned int *p =camera.renderData.matricesIndex+indexMatricesIndexCount;
        for (int j = 0; j < actualM.mesh.verticesCount/3; j++) {
            p[j] = i;
        }

        memcpy(camera.renderData.textureVerticesIndex + indexTextureVerticesIndexCount,
                actualM.mesh.textureVerticesIndex,
                sizeof(int) * actualM.mesh.textureVerticesIndexCount);


        memcpy(camera.renderData.normalIndex + indexNormalIndexCount,
                actualM.mesh.normalIndex,
                sizeof(int) * actualM.mesh.normalIndexCount);


        for(unsigned int j = 0; j < actualM.mesh.verticesIndexCount; j++)//PRECISO CORRIGIR TODOS OS INDEX
        {
            *(camera.renderData.verticesIndex + indexVerticesIndexCount + j) += camera.renderData.verticesIndexOffset;
        }
        for(unsigned int j = 0; j < actualM.mesh.textureVerticesIndexCount; j++)
        {
            *(camera.renderData.textureVerticesIndex + indexTextureVerticesIndexCount + j) += camera.renderData.textureIndexOffset;
        }
        for(unsigned int j = 0; j < actualM.mesh.normalIndexCount; j++)
        {
            *(camera.renderData.normalIndex + indexNormalIndexCount + j) += camera.renderData.normalIndexOffset;
        }

        indexVerticesIndexCount += actualM.mesh.verticesIndexCount;
        indexTextureVerticesIndexCount += actualM.mesh.textureVerticesIndexCount;
        indexNormalIndexCount += actualM.mesh.normalIndexCount;
        indexMatricesIndexCount += actualM.mesh.verticesCount/3;
        camera.renderData.verticesIndexOffset += actualM.mesh.verticesCount/3;
        camera.renderData.textureIndexOffset += actualM.mesh.textureVerticesCount/2;
        camera.renderData.normalIndexOffset += actualM.mesh.normalVecCount/3;
    }

    camera.renderData.textureIndxDATA = (unsigned int *)realloc(camera.renderData.textureIndxDATA,
                sizeof(int) * 2 * camera.renderData.models.size());

    for(unsigned short i = 0; i < TEXTURE_COUNT; i++){
        free(rmPerTex[i]);
    }

    free(antiqueMatrices);
}
