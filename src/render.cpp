
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
#define MOUSE_SENSI 0.05f

int fodase[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

void setTexParameter(){
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, LEVEL);
}

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
    renderFlags = (int*)malloc(0);

    for(int i =0; i < TEXTURE_COUNT; i++){
        modelsPerTex[i] = (unsigned short *)malloc(0);
    }
}


void RenderData::freeRenderData()
{
    for(int i = 0; i <models.size(); i++){
        models[i].mesh.deleteMesh();
    }
    for(int i =0; i < TEXTURE_COUNT; i++){
        free(modelsPerTex[i]);
    }
    free(textureIndxDATA);
    free(renderFlags);

    free(vertices);
    free(matrices);
    free(textureVertices);
    free(normalVec);

    free(verticesIndex);
    free(matricesIndex);
    free(textureVerticesIndex);
    free(normalIndex);
}

// rotation[0][0] = r.x; rotation[1][0] = r.y; rotation[2][0] = r.z;
// rotation[0][1] = u.x; rotation[1][1] = u.y; rotation[2][1] = u.z;
// rotation[0][2] = -f.x; rotation[1][2] = -f.y; rotation[2][2] = -f.z;
//
glm::vec4 Camera::getUp(){
    return translation* glm::vec4(rotation[0][1],rotation[1][1],rotation[2][1],rotation[3][1]);
}
glm::vec4 Camera::getFoward(){
    return translation* glm::vec4(rotation[0][2],rotation[1][2],rotation[2][2],rotation[3][2]);
}
glm::vec4 Camera::getRight(){
    return translation* glm::vec4(rotation[0][0],rotation[1][0],rotation[2][0],rotation[3][0]);
}
// glm::mat4 translation, rotation, localTranslation, viewMatrix, projMatrix;

Camera::Camera(glm::mat4 vMatrix,glm::mat4 pjMatrix):translation(1),rotation(1),localTranslation(1),viewMatrix(vMatrix),
                    projMatrix(pjMatrix),renderData(){}

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

    if (ImGui::IsWindowFocused() && ImGui::GetMousePos().y >=ImGui::GetCursorScreenPos().y)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)){
            ImGui::SetWindowFocus(NULL);
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        }

        ImGuiIO& io = ImGui::GetIO();

        ImVec2 mousePos = io.MousePos;
        ImVec2 windowCenter = ImGui::GetWindowSize();
        windowCenter.x *= .5;
        windowCenter.y *= .5;
        windowCenter = windowCenter +ImGui::GetWindowPos();

        ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        // printf("ca %f %f\n", io.MouseDelta.x,io.MouseDelta.y);
        ImVec2  mouseDelta = ImVec2(0,0);
        if(io.MouseDelta.x && io.MouseDelta.y)
            mouseDelta = ImVec2(mousePos.x - windowCenter.x,mousePos.y - windowCenter.y);


        glfwSetCursorPos(glfwWin,windowCenter.x,windowCenter.y);
        ImGui::Dummy(ImVec2(0,0));


        camera.rotation = glm::rotate(camera.rotation,glm::radians(MOUSE_SENSI*mouseDelta.x), glm::vec3(0, 1, 0));
        camera.rotation = glm::rotate(camera.rotation,glm::radians(-.5f*MOUSE_SENSI*mouseDelta.y), glm::vec3(1, 0, 0));
        float b = .3;
        if(camera.renderData.models.size() > camera.selectedModelIndex){
            glm::mat4 *modelMatrix = camera.getNMatrix(camera.selectedModelIndex);
            // if(ImGui::IsKeyPressed(ImGuiKey_H)){
                // rotation = glm::rotate(rotation,glm::radians(-10.0f), glm::vec3(1, 0, 0));
                // camera.viewMatrix = rotation * translation;
                // printf("\n %f %f %f %f \n %f %f %f %f \n %f %f %f %f \n %f %f %f %f \n",
                //     camera.viewMatrix[0][0],camera.viewMatrix[1][0],camera.viewMatrix[2][0],camera.viewMatrix[3][0],
                //     camera.viewMatrix[0][1],camera.viewMatrix[1][1],camera.viewMatrix[2][1],camera.viewMatrix[3][1],
                //     camera.viewMatrix[0][2],camera.viewMatrix[1][2],camera.viewMatrix[2][2],camera.viewMatrix[3][2],
                //     camera.viewMatrix[0][3],camera.viewMatrix[1][3],camera.viewMatrix[2][3],camera.viewMatrix[3][3]);
            // }
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
                camera.localTranslation = glm::translate(camera.localTranslation, b*glm::vec3(camera.getRight()));
                flags = flags | 4;

                // camera.viewMatrix[3][2] -= .2;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_D))
            {
                camera.localTranslation = glm::translate(camera.localTranslation, b*glm::vec3(-camera.getRight()));
                flags = flags | 4;

                // camera.viewMatrix[3][2] += .2;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_W))
            {
                camera.localTranslation = glm::translate(camera.localTranslation, b*glm::vec3(camera.getFoward()));
                flags = flags | 4;

                // camera.viewMatrix[3][0] -= .2;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_S))
            {
                camera.localTranslation = glm::translate(camera.localTranslation, b*glm::vec3(-camera.getFoward()));
                flags = flags | 4;

                // camera.viewMatrix[3][0] += .2;
            }
        }
        camera.translation = camera.localTranslation;
        camera.viewMatrix = camera.rotation * camera.translation;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        glfwSetWindowShouldClose(glfwWin, true);
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

    glGenBuffers(1, &EBO);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                        GL_FALSE, glm::value_ptr(camera.projMatrix));

//-------/-------/-------/----- SSBO -----/-------/-------/-------/-------/

    // ModelMatricesSSBO
    glGenBuffers(1, &modelMxSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMxSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMxSSBO);

    // TextureIndxSSBO
    glGenBuffers(1, &textureIndxSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureIndxSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, textureIndxSSBO);

    // TextureCoordSSBO
    glGenBuffers(1, &texCoordSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, texCoordSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, texCoordSSBO);

    // TextureIndexSSBO
    glGenBuffers(1, &texIndexSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, texIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, texIndexSSBO);

    // RenderFlagsSSBO
    glGenBuffers(1, &renderFlagsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderFlagsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, renderFlagsSSBO);

    // NormalVecsSSBO
    glGenBuffers(1, &normalVecsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, normalVecsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, normalVecsSSBO);

    // NormalVecsIndexSSBO
    glGenBuffers(1, &normalVecsIndexSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, normalVecsIndexSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, normalVecsIndexSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

//-------/-------/-------/----- FBOs -----/-------/-------/-------/-------/
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
                GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            texToShowFrom, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

//-------/-------/-------/----- Textures -----/-------/-------/-------/-------/

    for(int i = 0; i < TEXTURE_COUNT; i++){
        camera.renderData.texturesCount[i] = 0;
    }

    glGenTextures(TEXTURE_COUNT,texARRAY);
    for(unsigned short i = 0; i < TEXTURE_COUNT; i++){
        unsigned short k = pow(2,6+i);
        glBindTexture(GL_TEXTURE_2D_ARRAY,texARRAY[i]);
        glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,k,k,0,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

        setTexParameter();
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);



    glGenTextures(1, &texUtilitary);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texUtilitary);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,0,0,0,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    setTexParameter();
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

        m_VAO->bindVBO(1);
        glBufferData(GL_ARRAY_BUFFER,  sizeof(int) * camera.renderData.verticesCount/3,
                            camera.renderData.matricesIndex,GL_DYNAMIC_DRAW);
        m_VAO->bindVBO(0);
        glBufferData(GL_ARRAY_BUFFER,  sizeof(float) * camera.renderData.verticesCount,
                        camera.renderData.vertices,GL_DYNAMIC_DRAW);
    //-------/-------/-------/----- SSBO -----/-------/-------/-------/-------/
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

        // RenderFlagsSSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderFlagsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                    sizeof(int) * camera.renderData.models.size(),
                    camera.renderData.renderFlags, GL_DYNAMIC_DRAW);

        // TextureIndicesSSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureIndxSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                    sizeof(int) * 2 * camera.renderData.models.size(),
                    camera.renderData.textureIndxDATA, GL_DYNAMIC_DRAW);

        // TextureIndicesSSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, normalVecsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                    sizeof(float) * camera.renderData.normalVecCount,
                    camera.renderData.normalVec, GL_DYNAMIC_DRAW);

        // TextureIndicesSSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, normalVecsIndexSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                    sizeof(int) * camera.renderData.verticesIndexCount/3,
                    camera.renderData.normalIndex, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
    }
    else {
        m_VAO->bind();
        m_VAO->bindVBO(1);
        m_VAO->bindVBO(0);
    }

    for(unsigned short i = 0; i < TEXTURE_COUNT; i++){
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D_ARRAY,texARRAY[i]);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_FROM);

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
    m_VAO->unbindVBO();
    m_VAO->unbind();

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
                camera.selectedModelIndex = 0;
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



    ImGui::Begin("Render",nullptr);
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

    camera.translation[3][0] = 0;
    camera.translation[3][1] = 0;
    camera.translation[3][2] = 5;
    camera.rotation = glm::rotate(camera.rotation,3.14f, glm::vec3(0, 1, 0));

    ModelGenStruct models[3] =
    {
        ModelGenStruct
        {
            meshPath:"assets/3dmodels/Cube.obj",
            texPath:"assets/3dmodels/CubeTexture2.jpg",
            renderFlags:1
        },
        ModelGenStruct
        {
            meshPath:"assets/3dmodels/cannon_01_4k.obj",
            texPath:"assets/3dmodels/cannon_01_diff_4k.jpg",
            renderFlags:1

        },
        ModelGenStruct
        {
            meshPath:"assets/3dmodels/marble_bust_01_4k.obj",
            texPath:"assets/3dmodels/marble_bust_01_diff_4k.jpg",
            renderFlags:1
        }

    };

    // addModels(3, models);
    addModels(1, models);
    // addModels(1, &models[1]);

    // addModels(1, &models[1]);
    // unsigned short  a = 0;
    // rmModels(1, &a);

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
        // camera.viewMatrix  = rotation * translation;


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
                 totalTextureVerticesIndexCount = camera.renderData.textureVerticesIndexCount;

    for (unsigned short i = 0; i < n; i++){
        camera.renderData.models.push_back(Model(data[i].meshPath,data[i].texPath));
        Model &actualM = camera.renderData.models.back();
        actualM.mesh.renderFlags = data[i].renderFlags;
        totalVerticesCount += actualM.mesh.verticesCount;
        totalTextureVerticesCount += actualM.mesh.textureVerticesCount;
        totalNormalVecCount += actualM.mesh.normalVecCount;
        totalVerticesIndexCount += actualM.mesh.verticesIndexCount;
        totalTextureVerticesIndexCount += actualM.mesh.textureVerticesIndexCount;
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
                sizeof(int) * totalVerticesIndexCount/3);


    camera.renderData.textureIndxDATA = (unsigned int*)realloc(camera.renderData.textureIndxDATA,
                sizeof(int) * 2 * nModels);
    camera.renderData.renderFlags = (int*)realloc(camera.renderData.renderFlags,
                sizeof(int) * nModels);


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

        setTexParameter();

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

        memcpy(camera.renderData.normalIndex + camera.renderData.verticesIndexCount/3,
                actualM.mesh.normalIndex,
                sizeof(int) * actualM.mesh.verticesIndexCount/3);

        camera.renderData.renderFlags[nModels - n + i] = actualM.mesh.renderFlags;

        for(unsigned int j = 0; j < actualM.mesh.verticesIndexCount; j++){
            *(camera.renderData.verticesIndex + camera.renderData.verticesIndexCount + j) += camera.renderData.verticesIndexOffset;
        }
        for(unsigned int j = 0; j < actualM.mesh.textureVerticesIndexCount; j++){
            *(camera.renderData.textureVerticesIndex + camera.renderData.textureVerticesIndexCount + j) += camera.renderData.textureIndexOffset;
        }
        for(unsigned int j = 0; j < actualM.mesh.verticesIndexCount/3; j++){
            *(camera.renderData.normalIndex + camera.renderData.verticesIndexCount/3 + j) += camera.renderData.normalIndexOffset;
        }
        camera.renderData.verticesIndexCount += actualM.mesh.verticesIndexCount;
        camera.renderData.textureVerticesIndexCount += actualM.mesh.textureVerticesIndexCount;

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

            setTexParameter();

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
        actualM.mesh.deleteMesh();
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
                sizeof(int) * camera.renderData.verticesIndexCount / 3);


    camera.renderData.renderFlags = (int*)realloc(camera.renderData.renderFlags,
                sizeof(int) * camera.renderData.models.size());

    camera.renderData.verticesIndexOffset = 0;
    camera.renderData.textureIndexOffset = 0;
    camera.renderData.normalIndexOffset = 0;
    unsigned int indexVerticesCount = 0,indexVerticesIndexCount = 0,indexTextureVerticesCount = 0,
                 indexTextureVerticesIndexCount = 0,indexNormalVecCount = 0,indexMatricesIndexCount = 0;


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


        memcpy(camera.renderData.normalIndex + indexVerticesIndexCount/3,
                actualM.mesh.normalIndex,
                sizeof(int) * actualM.mesh.verticesIndexCount/3);

        camera.renderData.renderFlags[i] = actualM.mesh.renderFlags;


        for(unsigned int j = 0; j < actualM.mesh.verticesIndexCount; j++)//PRECISO CORRIGIR TODOS OS INDEX
        {
            *(camera.renderData.verticesIndex + indexVerticesIndexCount + j) += camera.renderData.verticesIndexOffset;
        }
        for(unsigned int j = 0; j < actualM.mesh.textureVerticesIndexCount; j++)
        {
            *(camera.renderData.textureVerticesIndex + indexTextureVerticesIndexCount + j) += camera.renderData.textureIndexOffset;
        }
        for(unsigned int j = 0; j < actualM.mesh.verticesIndexCount/3; j++)
        {
            *(camera.renderData.normalIndex + indexVerticesIndexCount/3 + j) += camera.renderData.normalIndexOffset;
        }

        indexVerticesIndexCount += actualM.mesh.verticesIndexCount;
        indexTextureVerticesIndexCount += actualM.mesh.textureVerticesIndexCount;

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
