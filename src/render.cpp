#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <stdlib.h>

#include "utils.hpp"
#include "vendor/glad/glad.h"//GLAD Always upper than GLFW
#include <GLFW/glfw3.h>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/stb_image/stb_image.h"

#include "render.hpp"
#include "model.hpp"
#include "shader.hpp"

#define VERTEX_SHADERS_LOCALPATH "vShader.vert"
#define FRAGMENT_SHADERS_LOCALPATH "fShader.frag"
#define GEOMETRY_SHADERS_LOCALPATH "gShader.geom"
#define LEVEL 0
#define MOUSE_SENSI 0.05f

#define MAX_RATIO_OF_EMPTY_TEXTURES .5
#define MIN_EMPTY_TEXTURES_COUNT_TO_RST 3
#define MAX_EMPTY_TEXTURES_COUNT 10

#define MATERIAL_CHANGE_FLAG 1 << 0
#define MATRICES_CHANGE_FLAG 1 << 1
#define MODELS_CHANGE_FLAG 1 << 2


int fodase[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

void sendWarning(const char *msg) {

}

void send(const char *msg) {

}

void sendError(const char *msg) {
    fprintf (stderr, "\n\nError: %s\n\n", msg);
}

void getShaderStatus(unsigned int shaderProgram,int status){
    int linkStatus;
    glGetShaderiv(shaderProgram, status, &linkStatus);
    if (!linkStatus) {
        GLint infoLogLength;
        glGetShaderiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *c = (char*)malloc(infoLogLength + 1);
        glGetShaderInfoLog(shaderProgram, infoLogLength, nullptr, c);
        c[infoLogLength] = 0;
        sendError(c);
        free(c);
    }
}

void getProgramStatus(unsigned int shaderProgram,int status){
    int linkStatus;
    glGetProgramiv(shaderProgram, status, &linkStatus);
    if (!linkStatus) {
        GLint infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *c = (char*)malloc(infoLogLength + 1);
        glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, c);
        c[infoLogLength] = 0;
        sendError(c);
        free(c);
    }
}

void setTexParameter(){
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, LEVEL);
}


RenderData::RenderData() : mesh()
{

    mesh.vertices = (float *)malloc(0);
    mesh.matrices = (float *)malloc(0);
    mesh.textureVertices = (float *)malloc(0);
    mesh.normalVertices = (float *)malloc(0);

    mesh.verticesIndex = (unsigned int *)malloc(0);
    mesh.modelParent = (unsigned int *)malloc(0);
    mesh.textureVerticesIndex = (unsigned int *)malloc(0);
    mesh.normalVerticesIndex = (unsigned int *)malloc(0);
    mesh.modelMaterial = (unsigned int *)malloc(0);

    for(unsigned int i = 0; i < TEXTURE_HANDLERS_COUNT; i++){
        textureHandlers[i].materialIndex = (unsigned int *)malloc(0);
        textureHandlers[i].texDimensions = pow(2, i +6);
        textureHandlers[i].emptyTextures = (unsigned int *)malloc(0);
        textureHandlers[i].emptyTexturesCount = 0;
    }

    materials = (Material *)malloc(0);
}


void RenderData::freeRenderData()
{
    for(unsigned int i = 0; i <models.size(); i++){
        models[i].mesh.deleteMesh();
    }
    for(unsigned int i =0; i < TEXTURE_HANDLERS_COUNT; i++){
        free(textureHandlers[i].materialIndex);
        free(textureHandlers[i].emptyTextures);

    }

    free(mesh.vertices);
    free(mesh.matrices);
    free(mesh.textureVertices);
    free(mesh.normalVertices);

    free(mesh.verticesIndex);
    free(mesh.modelParent);
    free(mesh.textureVerticesIndex);
    free(mesh.normalVerticesIndex);
    free(mesh.modelMaterial);

    free(materials);

}

// rotation[0][0] = r.x; rotation[1][0] = r.y; rotation[2][0] = r.z;
// rotation[0][1] = u.x; rotation[1][1] = u.y; rotation[2][1] = u.z;
// rotation[0][2] = -f.x; rotation[1][2] = -f.y; rotation[2][2] = -f.z;
//
glm::vec4 Camera::getUp(){
    return translation * glm::vec4(rotation[0][1],rotation[1][1],rotation[2][1],rotation[3][1]);
}
glm::vec4 Camera::getFoward(){
    return translation * glm::vec4(rotation[0][2],rotation[1][2],rotation[2][2],rotation[3][2]);
}
glm::vec4 Camera::getRight(){
    return translation * glm::vec4(rotation[0][0],rotation[1][0],rotation[2][0],rotation[3][0]);
}
glm::vec4 Camera::getPosition(){
    return translation * glm::vec4(0,0,0,1);
}


Camera::Camera():translation(1), rotation(1), localTranslation(1)
{
    projMatrix = glm::perspective(glm::radians(45.0f),
                                    (float)SCR_X / (float)SCR_Y, 0.1f, 100.0f);
    viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 *RenderData::getNMatrix(unsigned short index){
    return (glm::mat4*)(mesh.matrices + 16*index);
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




Render::Render(GLFWwindow *win):  camera(),
            shader((const char *[3]){
                VERTEX_SHADERS_LOCALPATH,
                FRAGMENT_SHADERS_LOCALPATH,
                GEOMETRY_SHADERS_LOCALPATH}),glfwWin(win) {

    shaderProgram = glCreateProgram();
    shader.attach(shaderProgram);

    if (transFeed) {
        const GLchar *feedbackVaryings[] = {"transformFeedback"};
        glTransformFeedbackVaryings(shaderProgram, 1, feedbackVaryings,
                                    GL_INTERLEAVED_ATTRIBS);

        feedbacknumber = 4 * renderData.mesh.verticesIndexCount;
        feedbacksize = feedbacknumber * sizeof(float);
    }

    glLinkProgram(shaderProgram);
    getProgramStatus(shaderProgram,GL_LINK_STATUS);
    glUseProgram(shaderProgram);

}

Render::~Render() {
    renderData.freeRenderData();
    if (transFeed) {
        glDeleteQueries(1,&QUERY);
        glDeleteBuffers(1, &TBO);
    }
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(SSBO_COUNT,ssbos);
    glDeleteBuffers(VBO_COUNT,VBOS);
    glDeleteFramebuffers(1, &FBO_FROM);
    glDeleteFramebuffers(1, &FBO_TO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1,&texToRenderOver);
    glDeleteTextures(1,&texToShowFrom);
    glDeleteProgram(shaderProgram);
}

MaterialGenData a;


void Render::input()
{
    if(ImGui::IsMouseDragging(ImGuiMouseButton_Left)){
        printf("bahkk\n");
    }
    if (ImGui::IsWindowFocused() && ImGui::GetMousePos().y >=ImGui::GetCursorScreenPos().y)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)){
            ImGui::SetWindowFocus(nullptr);
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
        // printf("ba:%f  kk %f\n",mouseDelta.x,mouseDelta.y);
        camera.angle.x += MOUSE_SENSI*mouseDelta.x;
        camera.angle.y += .5f*MOUSE_SENSI*mouseDelta.y;
        camera.rotation = glm::rotate(glm::mat4(1),glm::radians(camera.angle.x), glm::vec3(0, 1, 0));
        camera.rotation = glm::rotate(camera.rotation,glm::radians(camera.angle.y), glm::vec3(camera.getRight()));
        float b = .3;
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)){

            printf("batata\n\n\n");
            allocMaterial(a);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)){
            if(renderData.materialsCount > 0){
                freeMaterial(renderData.materialsCount-1);
                renderData.materialsCount--;
            }
        }


        if (ImGui::IsKeyPressed(ImGuiKey_A)){
            camera.localTranslation = glm::translate(camera.localTranslation, b*glm::vec3(camera.getRight()));
            flags = flags | 4;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_D)){
            camera.localTranslation = glm::translate(camera.localTranslation, b*glm::vec3(-camera.getRight()));
            flags = flags | 4;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_W)){
            camera.localTranslation = glm::translate(camera.localTranslation, b*glm::vec3(camera.getFoward()));
            flags = flags | 4;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_S)){
            camera.localTranslation = glm::translate(camera.localTranslation, b*glm::vec3(-camera.getFoward()));
            flags = flags | 4;
        }

        if(renderData.models.size() > selectedModelIndex){
            glm::mat4 *modelMatrix = renderData.getNMatrix(selectedModelIndex);

            if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)){
                *modelMatrix =
                    glm::rotate(*modelMatrix, glm::radians(-10.0f), glm::vec3(1, 0, 0));
                flags = flags | 4;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)){
                *modelMatrix =
                    glm::rotate(*modelMatrix, glm::radians(2.0f), glm::vec3(1, 0, 0));
                flags = flags | 4;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)){
                *modelMatrix =
                    glm::rotate(*modelMatrix, glm::radians(-10.0f), glm::vec3(0, 1, 0));
                flags = flags | 4;

            }
            if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)){
                *modelMatrix =
                    glm::rotate(*modelMatrix, glm::radians(2.0f), glm::vec3(0, 1, 0));
                flags = flags | 4;
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
        glGenQueries(1, &QUERY);
        glGenBuffers(1, &TBO);
    }

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glGenBuffers(2,VBOS);
    glBindBuffer(GL_ARRAY_BUFFER,VBOS[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER,VBOS[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(int),
                            (void *)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenBuffers(1, &EBO);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                        GL_FALSE, glm::value_ptr(camera.projMatrix));


    glGenBuffers(SSBO_COUNT, ssbos);
    for(unsigned int i = 0; i < SSBO_COUNT; i++){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[i]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, ssbos[i]);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


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

    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 2;
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
    // glm::vec4 po = glm::inverse(camera.viewMatrix)*glm::vec4(0,0,0,1); //w = 1bcs is a point
    // glUniform4f(glGetUniformLocation(shaderProgram,"cameraPosition"),po.x,po.y,po.z,po.w);

}


void Render::renderDrawing()
{
    if(flags & MATRICES_CHANGE_FLAG){
        flags = flags & ~(MATRICES_CHANGE_FLAG);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[SSBOS::ModelMatricesSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        16 * sizeof(float) * renderData.mesh.meshesCount,
                        renderData.mesh.matrices,GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    if(flags & MATERIAL_CHANGE_FLAG){
        flags = flags & ~(MATERIAL_CHANGE_FLAG);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[SSBOS::MaterialsSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(Material) * renderData.materialsCount,
                        renderData.materials,GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[1]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


    for(unsigned short i = 0; i < TEXTURE_HANDLERS_COUNT; i++){
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D_ARRAY,renderData.textureHandlers[i].texture);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_FROM);

    if (transFeed) {
        glBindBuffer(GL_ARRAY_BUFFER, TBO);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, TBO);
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, QUERY);
        glBeginTransformFeedback(GL_TRIANGLES);
    }

    glDrawElements(GL_TRIANGLES, renderData.mesh.verticesIndexCount, GL_UNSIGNED_INT,
                    0);

    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    if(transFeed) {
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
}


void Render::imguiSetting()
{
    ImGui::NewFrame();
    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Pinto");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if(ImGui::Begin("Assets")) {
        ImGui::End();
    }
    if(ImGui::Begin("Models")) {
        if(renderData.models.size() > selectedModelIndex) {
            glm::vec4 pintobom = renderData.models[selectedModelIndex].mesh.matrix * glm::vec4(0,0,0,1);
            ImGui::Text("Selected Model Center:(%f, %f, %f)",pintobom.x,pintobom.y,pintobom.z);
        }

        if(ImGui::Button("Add")) {
            if(selectedModelIndex < renderData.models.size()) {
                const char *meshPath = renderData.models[selectedModelIndex].mesh.meshPath;
            }
        }
        if(ImGui::Button("Remove")) {
            if(renderData.models.size() > selectedModelIndex) {
                printf("aa%d\n", selectedModelIndex);
                selectedModelIndex = 0;
            }

        }
        if(ImGui::BeginTable("Modelss1", 1)) {
            char label[100];
            for(unsigned int i = 0; i < renderData.models.size(); i++){
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                sprintf(label, "Model %d",i);
                if(i == selectedModelIndex){
                    ImGui::TableSetBgColor(
                        ImGuiTableBgTarget_RowBg0,
                        IM_COL32(38, 38, 38, 255)
                    );
                }
                if(ImGui::Selectable(label)){
                    selectedModelIndex = i;
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
    a.K[0] = glm::vec3(1,0,0);
    a.K[1] = glm::vec3(0,1,0);
    a.K[2] = glm::vec3(0,0,1);
    a.Ni = 0;
    a.bm = 0;
    a.d = 0;
    a.maps[0]  = "assets/3dmodels/lemon_diff_4k.jpg";
    // a.maps[1]  = "assets/3dmodels/a.raw";
    // a.maps[2]  = "assets/3dmodels/a.raw";
    a.type = SOLID_COLOR;
    once();

    camera.translation[3][0] = 0;
    camera.translation[3][1] = 0;
    camera.translation[3][2] = 5;
    camera.rotation = glm::rotate(camera.rotation,3.14f, glm::vec3(0, 1, 0));

    a.type = TEXTURE;

    // pushModels(MeshGenData{
    //     "assets/3dmodels/Cube.obj"
    // },a);
    // pushModels(MeshGenData{
    //     "assets/3dmodels/Cube.obj"
    // },a);
    // a.maps[0] = "assets/3dmodels/cannon_01_diff_4k.jpg";
    pushModels(MeshGenData{
        "assets/3dmodels/cannon_01_4k.obj"
    },a);
    // a.maps[0] = "assets/3dmodels/marble_bust_01_diff_4k.jpg";
    pushModels(MeshGenData{
        "assets/3dmodels/marble_bust_01_4k.obj"
    },a);
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
        updatePipeline();
    }
}

void Render::updatePipeline(){
    if(minFramesToUp == -1)
        return;
    if(minFramesToUp > 0){
        minFramesToUp--;
        return;
    }
    if(!(flags & MODELS_CHANGE_FLAG))
        return;
    flags = flags & ~(MODELS_CHANGE_FLAG);
    flags = flags | MATRICES_CHANGE_FLAG;
    // unsigned int  verticesCount = 0, verticesIndexCount = 0, textureVerticesCount=0,
    //                     textureVerticesIndexCount=0,normalVerticesCount=0,// normalIndexCount = nOfPrimitives = verticesIndexCount /3
    //                     verticesIndexOffset= 0,textureIndexOffset = 0,normalIndexOffset=0;
    // float *vertices,*matrices,*textureVertices,*normalVertices;
    // unsigned int *verticesIndex,*modelParent,*textureVerticesIndex,*normalVerticesIndex,*modelsMaterials;

    renderData.mesh.verticesCount = 0;
    renderData.mesh.verticesIndexCount = 0;
    renderData.mesh.textureVerticesCount = 0;
    renderData.mesh.textureVerticesIndexCount = 0;
    renderData.mesh.normalVerticesCount = 0;

    renderData.mesh.verticesIndexOffset = 0;
    renderData.mesh.textureIndexOffset = 0;
    renderData.mesh.normalIndexOffset = 0;

    for(unsigned int i = 0; i < renderData.models.size(); i++){
        Model actualModel = renderData.models[i];
        renderData.mesh.verticesCount += actualModel.mesh.verticesCount;
        renderData.mesh.verticesIndexCount += actualModel.mesh.verticesIndexCount;
        renderData.mesh.textureVerticesCount += actualModel.mesh.textureVerticesCount;
        renderData.mesh.textureVerticesIndexCount += actualModel.mesh.textureVerticesIndexCount;
        renderData.mesh.normalVerticesCount += actualModel.mesh.normalVerticesCount;
    }

    renderData.mesh.meshesCount = renderData.models.size();

    renderData.mesh.vertices = (float*)realloc(renderData.mesh.vertices,
                        sizeof(float) * renderData.mesh.verticesCount);
    renderData.mesh.matrices = (float*)realloc(renderData.mesh.matrices,
                        sizeof(float) * 16 * renderData.mesh.meshesCount);
    renderData.mesh.textureVertices = (float*)realloc(renderData.mesh.textureVertices,
                        sizeof(float) * renderData.mesh.textureVerticesCount);
    renderData.mesh.normalVertices = (float*)realloc(renderData.mesh.normalVertices,
                        sizeof(float) * renderData.mesh.normalVerticesCount);

    renderData.mesh.verticesIndex = (unsigned int*)realloc(renderData.mesh.verticesIndex,
                        sizeof(unsigned int) * renderData.mesh.verticesIndexCount);
    renderData.mesh.modelParent = (unsigned int*)realloc(renderData.mesh.modelParent,
                        sizeof(unsigned int) * renderData.mesh.verticesCount / 3);
    renderData.mesh.textureVerticesIndex = (unsigned int*)realloc(renderData.mesh.textureVerticesIndex,
                        sizeof(unsigned int) * renderData.mesh.textureVerticesIndexCount);
    renderData.mesh.normalVerticesIndex = (unsigned int*)realloc(renderData.mesh.normalVerticesIndex,
                        sizeof(unsigned int) * renderData.mesh.verticesIndexCount / 3);
    renderData.mesh.modelMaterial = (unsigned int*)realloc(renderData.mesh.modelMaterial,
                        sizeof(unsigned int) * renderData.mesh.meshesCount);


    unsigned int localVerticesNumber = 0;
    unsigned int localVerticesIndexNumber = 0;
    unsigned int localTextureVerticesNumber = 0;
    unsigned int localTextureVerticesIndexNumber = 0;
    unsigned int localNormalVerticesNumber = 0;
    for(unsigned int i = 0; i < renderData.models.size(); i++){
        Mesh actualMesh = renderData.models[i].mesh;
        memcpy(renderData.mesh.vertices + localVerticesNumber,
            actualMesh.vertices, sizeof(float) * actualMesh.verticesCount);
        memcpy(renderData.mesh.matrices + 16 *  i,
            glm::value_ptr(actualMesh.matrix), sizeof(float) * 16);
        memcpy(renderData.mesh.textureVertices + localTextureVerticesNumber,
            actualMesh.textureVertices, sizeof(float) * actualMesh.textureVerticesCount);
        memcpy(renderData.mesh.normalVertices + localNormalVerticesNumber,
            actualMesh.normalVertices, sizeof(float) * actualMesh.normalVerticesCount);



        memcpy(renderData.mesh.verticesIndex + localVerticesIndexNumber,
            actualMesh.verticesIndex, sizeof(unsigned int) * actualMesh.verticesIndexCount);
        memcpy(renderData.mesh.textureVerticesIndex + localTextureVerticesIndexNumber,
            actualMesh.textureVerticesIndex, sizeof(unsigned int) * actualMesh.textureVerticesIndexCount);
        memcpy(renderData.mesh.normalVerticesIndex + localVerticesIndexNumber / 3,
            actualMesh.normalVerticesIndex, sizeof(unsigned int) * actualMesh.verticesIndexCount / 3);


        renderData.mesh.modelMaterial[i] = renderData.models[i].materialIndex;

        for(unsigned int j = 0; j < actualMesh.verticesCount / 3; j++){
            renderData.mesh.modelParent[j + localVerticesNumber / 3] = i;
        }

        for(unsigned int j = 0; j < actualMesh.verticesIndexCount; j++){
            *(renderData.mesh.verticesIndex + localVerticesIndexNumber + j) += localVerticesNumber;
        }
        for(unsigned int j = 0; j < actualMesh.textureVerticesIndexCount; j++){
            *(renderData.mesh.textureVerticesIndex + localVerticesIndexNumber + j) += localTextureVerticesNumber;
        }
        for(unsigned int j = 0; j < actualMesh.verticesIndexCount / 3; j++){
            *(renderData.mesh.normalVerticesIndex + localVerticesIndexNumber/3 + j) += localNormalVerticesNumber;
        }


        localVerticesNumber += actualMesh.verticesCount;
        localVerticesIndexNumber += actualMesh.verticesIndexCount;
        localTextureVerticesNumber += actualMesh.textureVerticesCount;
        localTextureVerticesIndexNumber += actualMesh.textureVerticesIndexCount;
        localNormalVerticesNumber += actualMesh.normalVerticesCount;
    }

    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,ssbos[SSBOS::NormalVecsIndexSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(unsigned int) * renderData.mesh.verticesIndexCount/3,
                        renderData.mesh.normalVerticesIndex,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,ssbos[SSBOS::NormalVecsSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(float) * renderData.mesh.normalVerticesCount,
                        renderData.mesh.normalVertices,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,ssbos[SSBOS::TextureIndexSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(unsigned int) * renderData.mesh.textureVerticesIndexCount,
                        renderData.mesh.textureVerticesIndex,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,ssbos[SSBOS::TextureCoordSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(float) * renderData.mesh.textureVerticesCount,
                        renderData.mesh.textureVertices,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,ssbos[SSBOS::ModelsMaterialsSSBO]);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                        sizeof(unsigned int) * renderData.mesh.meshesCount,
                        renderData.mesh.modelMaterial,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        sizeof(unsigned int) * renderData.mesh.verticesIndexCount,
                        renderData.mesh.verticesIndex,GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBOS[0]);
        glBufferData(GL_ARRAY_BUFFER,  sizeof(float) * renderData.mesh.verticesCount,
                            renderData.mesh.vertices,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER,VBOS[1]);
        glBufferData(GL_ARRAY_BUFFER,  sizeof(int) * renderData.mesh.verticesCount/3,
                            renderData.mesh.modelParent,GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }
}

// WW:widht
// HH:height
// BB: bpp
//0xFFRAWIMGWWHHBB
//0123
TextureLocation RenderData::addTexToHandler(char *localPath,bool toProcess) {
    char *path = localPath;
    if(toProcess) {
        int i = 0,k;
        while(path[i] != 0) {
            if(path[i] == '.')
                k = i;
            i++;
        }
        char *newPath = (char*)malloc(k+RAW_TEX_EXTENSION_LENGHT+2);//path + '.' + RAW_TEX_EXTENSION + '\0'
        memcpy(newPath, path, k+1);
        for(int j = 0; j < RAW_TEX_EXTENSION_LENGHT; j++) {
            newPath[k+1+j] = RAW_TEX_EXTENSION[j];
        }
        newPath[k+1+RAW_TEX_EXTENSION_LENGHT] = 0;
        if(access(newPath,F_OK)) {
            int width, height,bpp;
            stbi_set_flip_vertically_on_load (1);
            //localBuffeSize = 4 * width * width
            unsigned char *localBuffer = stbi_load(path, &width,  &height, &bpp, 4);
            if(localPath == nullptr){
                sendError("image do not exist");
                exit(EXIT_FAILURE);
            }
            if(width != height) {
                sendError("Width != Height");
                exit(EXIT_FAILURE);
            }
            if(width > 4096) {
                sendError("Width,Height > 4096");
                exit(EXIT_FAILURE);
            }

            path = newPath;
            FILE *file = fopen(path,"wb");
            char c[14];//13 is the lenght of 0xFFRAWIMGWWHHBB
            sprintf(c, "\xffRAWIMG");
            fwrite(c, sizeof(char), 7, file);
            char f[6];
            f[0] = (unsigned char)width;
            f[1] = (unsigned char)(width >> 8);
            f[2] = (unsigned char)height;
            f[3] = (unsigned char)(height >> 8);
            f[4] = (unsigned char)bpp;
            f[5] = (unsigned char)(bpp >> 8);
            fwrite(f, sizeof(char), 6, file);

            fwrite(localBuffer, 1, 4 * width * width, file);
            fclose(file);
        }
        else {
            path = newPath;
            // free(newPath);
        }
    }


    // i - k - 1;
    // stbi_set_flip_vertically_on_load (1);
    unsigned int lenght;
    unsigned char* localBuffer = (unsigned char*)readFile(path, &lenght,"rb");
    if(!matchPairs(localBuffer, 0, (unsigned char[]){(unsigned char)'\xff','R','A','W','I','M','G'})) {
        sendError("addTexToHandler, file of wrong type");
        return TextureLocation();
    }

    unsigned short width = *((unsigned short*)(localBuffer + 7));
    // unsigned short height = *((unsigned short*)(localBuffer + 9));
    // unsigned short bpp = *((unsigned short*)(localBuffer + 11));


    unsigned char handlerIndex = (unsigned char)floor(log2(width)-6);
    unsigned int index = textureHandlers[handlerIndex].addTex((unsigned char*)localBuffer+13);
    free(localBuffer);
    if(toProcess){
        free(path);
    }
    return TextureLocation {
        handler: handlerIndex,
        index: index
    };
}

unsigned int TextureHandler::addTex(unsigned char *localBuffer){
    if(emptyTexturesCount) {
        unsigned int i = emptyTextures[0];
        glBindTexture(GL_TEXTURE_2D_ARRAY,texture);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, LEVEL, 0, 0,
                            i-1, texDimensions, texDimensions, 1, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);

        emptyTexturesCount--;
        emptyTextures = (unsigned int*)realloc(emptyTextures, emptyTexturesCount * sizeof(int));
        return i;
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, *texUtilitary);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,texDimensions,texDimensions,texturesCount+1,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);

    setTexParameter();

    glCopyImageSubData(texture,GL_TEXTURE_2D_ARRAY,LEVEL,0,0,0,*texUtilitary,GL_TEXTURE_2D_ARRAY,LEVEL,
                    0,0,0,texDimensions,texDimensions,texturesCount);

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, LEVEL, 0, 0,
                        texturesCount, texDimensions, texDimensions, 1, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);//texturesCount or texturesCount-1 ?

    glBindTexture(GL_TEXTURE_2D_ARRAY,texture);

    glTexImage3D(GL_TEXTURE_2D_ARRAY,LEVEL,GL_RGBA8 ,0,0,0,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    glBindTexture(GL_TEXTURE_2D_ARRAY,0);

    texturesCount++;

    unsigned int cache = texture;
    texture = *texUtilitary;
    *texUtilitary = cache;

    return texturesCount-1;
}

void TextureHandler::rmTex(unsigned int index) {
    emptyTexturesCount++;
    if(emptyTexturesCount > MAX_EMPTY_TEXTURES_COUNT ||
        (emptyTexturesCount > MIN_EMPTY_TEXTURES_COUNT_TO_RST &&
            emptyTexturesCount > texturesCount * MAX_RATIO_OF_EMPTY_TEXTURES)) {
        printf("bahhh\n\n\n\n");
        return;
    }
    emptyTextures = (unsigned int*)realloc(emptyTextures, emptyTexturesCount);
    emptyTextures[emptyTexturesCount-1] = index;
}

unsigned int Render::pushModels(MeshGenData mesh,unsigned int materialIndex) {
    flags = flags | MODELS_CHANGE_FLAG;
    minFramesToUp = 0;
    renderData.models.push_back(Model(Mesh(mesh),materialIndex));
    return renderData.models.size() -1;
}

unsigned int Render::pushModels(MeshGenData mesh,MaterialGenData material) {
    return pushModels(mesh,allocMaterial(material));
}

unsigned int Render::pushModels(MeshGenData mesh) {
    return pushModels(mesh,0);
}

void Render::popModels(unsigned int index) {
    flags = flags | MODELS_CHANGE_FLAG;
    renderData.models.erase(renderData.models.begin() + index);
}


unsigned int Render::allocMaterial(MaterialGenData data){
    renderData.materialsCount++;
    renderData.materials = (Material*)realloc(renderData.materials, sizeof(Material) * renderData.materialsCount);
    Material *mater = renderData.materials + renderData.materialsCount-1;

    mater->Ni = data.Ni;
    mater->bm = data.bm;
    mater->d = data.d;
    mater->type  = data.type;
    for(int i = 0; i < 3; i++){
        mater->K[3 * i] = data.K[i].x;
        mater->K[3 * i + 1] = data.K[i].y;
        mater->K[3 * i + 2] = data.K[i].z;

        if(data.type == TEXTURE && data.maps[i] != nullptr)
            mater->maps[i] = renderData.addTexToHandler(data.maps[i]);
    }
    if(data.maps[3] != nullptr)
        mater->maps[3] = renderData.addTexToHandler(data.maps[3]);

    flags = flags | MATERIAL_CHANGE_FLAG;
    return renderData.materialsCount - 1;
}

void Render::freeMaterial(unsigned int index){
    Material *newMat = (Material*)malloc(sizeof(Material) * (renderData.materialsCount - 1));
    if(index > 0){
        memcpy(newMat, renderData.materials,index);
    }
    if(index > renderData.materialsCount - 1){
        memcpy(newMat+index+1, renderData.materials,renderData.materialsCount - 1 - index);
    }
    free(renderData.materials);
    renderData.materials = newMat;
    renderData.materialsCount--;
    flags = flags | MATERIAL_CHANGE_FLAG;
}
