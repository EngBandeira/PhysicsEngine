
#include "render.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "vertexArray.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define SCR_X 800
#define SCR_Y 600

#define VERTEX_SHADERS_LOCALPATH "vShader.vert"
#define FRAGMENT_SHADERS_LOCALPATH "fShader.frag"
#define GEOMETRY_SHADERS_LOCALPATH "gShader.geom"


RenderData::RenderData() : models() {
  vertices = (float *)malloc(0);
  matrices = (float *)malloc(0);

  matricesIndex = (unsigned int *)malloc(0);
  verticesIndex = (unsigned int *)malloc(0);

  verticesPerModels = (int *)malloc(0);
}

RenderData::~RenderData() {
  // free(vertices);
  // free(matrices);
  // free(matricesIndex);
  // free(verticesIndex);
}

Camera::Camera(glm::mat4 vMatrix,glm::mat4 pjMatrix):viewMatrix(vMatrix),
                    projMatrix(pjMatrix),renderData()
{

}

glm::mat4 *Camera::getNMatrix(unsigned short index){
    return (glm::mat4*)(renderData.matrices + 16*index);
}


Render::Render(glm::mat4 vMatrix, glm::mat4 pjMatrix,GLFWwindow *win)
    : camera(vMatrix,pjMatrix),
      m_VAO(std::make_unique<VAO>()),
      shader((const char *[3]){VERTEX_SHADERS_LOCALPATH,
                               FRAGMENT_SHADERS_LOCALPATH,
                               GEOMETRY_SHADERS_LOCALPATH}),glfwWin(win)
      {
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
  if (transFeed) {
    glDeleteBuffers(1, &TBO);
  }
  m_VAO.reset();
  glDeleteProgram(shaderProgram);
  glDeleteFramebuffers(1, &FBO);
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
        glm::mat4 *modelMatrix = camera.getNMatrix(camera.selectedModelIndex);
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
        {
            *modelMatrix =
                glm::rotate(*modelMatrix, glm::radians(-10.0f), glm::vec3(1, 0, 0));
        }
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
        {
            *modelMatrix =
                glm::rotate(*modelMatrix, glm::radians(2.0f), glm::vec3(1, 0, 0));
        }
        if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
        {
            *modelMatrix =
                glm::rotate(*modelMatrix, glm::radians(-10.0f), glm::vec3(0, 1, 0));
        }
        if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
        {
            *modelMatrix =
                glm::rotate(*modelMatrix, glm::radians(2.0f), glm::vec3(0, 1, 0));
        }
        if (ImGui::IsKeyPressed(ImGuiKey_A))
        {
            *modelMatrix = glm::translate(*modelMatrix, glm::vec3(1, 0, 0));
            // camera.viewMatrix[3][2] -= .2;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_D))
        {
            *modelMatrix = glm::translate(*modelMatrix, glm::vec3(-1, 0, 0));

             // camera.viewMatrix[3][2] += .2;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_W))
        {
            *modelMatrix = glm::translate(*modelMatrix, glm::vec3(0, 0, 1));

             // camera.viewMatrix[3][0] -= .2;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_S))
        {
            *modelMatrix = glm::translate(*modelMatrix, glm::vec3(0, 0, -1));

             // camera.viewMatrix[3][0] += .2;
        }

        if (ImGui::IsKeyPressed(ImGuiKey_E))
        {
            camera.selectedModelIndex++;
            printf("%d\n", camera.selectedModelIndex);
        }if (ImGui::IsKeyPressed(ImGuiKey_Q))
        {
            camera.selectedModelIndex--;
            printf("%d\n", camera.selectedModelIndex);

        }
    }
}

void Render::predraw()
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

    // set vec3 position
    // for(int i = 0; i < (camera.renderData.verticesCount / 3); i++){

    //     printf(
    //         "(%f, %f, %f, %d)\n ",
    //         camera.renderData.vertices[3*i],
    //         camera.renderData.vertices[3*i+1],
    //         camera.renderData.vertices[3*i+2],
    //         camera.renderData.matricesIndex[i]
    //     );
    // }

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
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                16 * sizeof(float) * camera.renderData.models.size(),
                camera.renderData.matrices, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, modelMxSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind


    // EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                sizeof(unsigned int) * camera.renderData.verticesIndexCount,
                camera.renderData.verticesIndex, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1,
                        GL_FALSE, glm::value_ptr(camera.projMatrix));

    // model.tex.Bind(1);
    // glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 1);
    // glfwSetKeyCallback(bigWindow, processInput);
    // model.tex.Unbind();

    // FBO
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // TEX1
    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_X, SCR_Y, 0, GL_RGB,
                GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            renderTex, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_X, SCR_Y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                GL_RENDERBUFFER, RBO);
    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Render::newframe() {
  glClearColor(0.7f, 0.3f, 0.17f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  glClearColor(0.07f, 0.13f, 0.27f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ImGui_ImplGlfw_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  // glUseProgram(shaderProgram);

  ImGui::NewFrame();
  ImGui::Begin("Render");
  input();
  ImGui::GetWindowDrawList()->AddImage((void *)renderTex, ImGui::GetWindowPos(),
                                       ImGui::GetWindowSize() +
                                           ImGui::GetWindowPos(),
                                       ImVec2(0, 1), ImVec2(1, 0));

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  glBindTexture(GL_TEXTURE_2D, renderTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImGui::GetWindowSize().x,
               ImGui::GetWindowSize().y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         renderTex, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                        ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ImGui::End();

  bool a = 1;
  ImGui::ShowDemoWindow(&a);

  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE,
                     glm::value_ptr(camera.viewMatrix));
}

void Render::draw() {
  // model.tex.Bind(1);

  m_VAO->bind();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                  sizeof(unsigned int) * camera.renderData.verticesIndexCount,
                  camera.renderData.verticesIndex);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  //Repassing ModelsMatricesSSBO
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMxSSBO);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                  16 * sizeof(float) * camera.renderData.models.size(),
                  camera.renderData.matrices);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  m_VAO->bindVBO(1);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(int) * camera.renderData.verticesCount/3,
                    camera.renderData.matricesIndex);
  m_VAO->bindVBO(0);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * camera.renderData.verticesCount,
                  camera.renderData.vertices);




  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glClearColor(0.7f, 0.3f, 0.17f, 1.0f);

  if (transFeed) {
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, TBO);
    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, Query);
    glBeginTransformFeedback(GL_TRIANGLES);
  }
  glDrawElements(GL_TRIANGLES, camera.renderData.verticesIndexCount, GL_UNSIGNED_INT,
                 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  m_VAO->unbind();
  // model.tex.Unbind();

  if (transFeed) {
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

void Render::addModels(unsigned short n, Model *data) {
  for (unsigned short i = 0; i < n; i++) {
    Model actualM = data[i];
    for(int j = 0; j < actualM.mesh.verticesIndexCount; j++){
        actualM.mesh.verticesIndex[j] += camera.renderData.verticesOffsetIndex;
    }
    camera.renderData.verticesOffsetIndex += data->mesh.verticesCount/3;

    camera.renderData.models.push_back(actualM);
    unsigned long nModels = camera.renderData.models.size();

    camera.renderData.matrices =
        (float *)realloc(camera.renderData.matrices, sizeof(float) * 16 * nModels);
    memcpy(camera.renderData.matrices + 16 * (nModels - 1),
           glm::value_ptr(actualM.mesh.matrix), 16 * sizeof(float));

    camera.renderData.vertices = (float *)realloc(
        camera.renderData.vertices, sizeof(float) * (camera.renderData.verticesCount +
                                              actualM.mesh.verticesCount));
    memcpy(camera.renderData.vertices + camera.renderData.verticesCount,
           actualM.mesh.vertices, sizeof(float) * actualM.mesh.verticesCount);
    camera.renderData.verticesCount += actualM.mesh.verticesCount;


    camera.renderData.verticesIndex = (unsigned int *)realloc(
        camera.renderData.verticesIndex,
        sizeof(int) *
            (camera.renderData.verticesIndexCount + actualM.mesh.verticesIndexCount));
    memcpy(camera.renderData.verticesIndex + camera.renderData.verticesIndexCount,
           actualM.mesh.verticesIndex,
           sizeof(int) * actualM.mesh.verticesIndexCount);
    camera.renderData.verticesIndexCount += actualM.mesh.verticesIndexCount;

    camera.renderData.matricesIndex = (unsigned int *)realloc(
        camera.renderData.matricesIndex, sizeof(int) * camera.renderData.verticesCount/3);
    unsigned int *p =camera.renderData.matricesIndex+(camera.renderData.verticesCount- actualM.mesh.verticesCount)/3;
    for (int i = 0; i < actualM.mesh.verticesCount/3; i++) {
      p[i] = nModels - 1;
    }
    // printf("caraio %d  de %d\n",nModels-1,);
  }
}

void Render::rmModels(unsigned short n, unsigned short *index)
{
    for (unsigned short i = 0; i < n; i++)
    {
        Model actualM = camera.renderData.models.at(index[i]);
        camera.renderData.models.erase(camera.renderData.models.begin() + index[i]);
        camera.renderData.verticesCount -= actualM.mesh.verticesCount;
        camera.renderData.verticesIndexCount -= actualM.mesh.verticesIndexCount;
    }
    camera.renderData.matrices = (float *)realloc(
        camera.renderData.matrices, sizeof(float) * 16 * camera.renderData.models.size());
    camera.renderData.vertices = (float *)realloc(
        camera.renderData.vertices, sizeof(float) * camera.renderData.verticesCount);
    camera.renderData.verticesIndex = (unsigned int *)realloc(
        camera.renderData.verticesIndex, sizeof(int) * camera.renderData.verticesIndexCount);
    for (unsigned short i = 0; i < camera.renderData.models.size(); i++) {
        Model actualM = camera.renderData.models.at(i);

        memcpy(camera.renderData.matrices + 16 * i, glm::value_ptr(actualM.mesh.matrix),
            16 * sizeof(float));

        memcpy(camera.renderData.vertices + camera.renderData.verticesCount,
            actualM.mesh.vertices, sizeof(float) * actualM.mesh.verticesCount);
        camera.renderData.verticesCount += actualM.mesh.verticesCount;

        memcpy(camera.renderData.verticesIndex + camera.renderData.verticesIndexCount,
            actualM.mesh.verticesIndex,
            sizeof(int) * actualM.mesh.verticesIndexCount);
        camera.renderData.verticesIndexCount += actualM.mesh.verticesIndexCount;

        camera.renderData.matricesIndex = (unsigned int *)realloc(
            camera.renderData.matricesIndex, sizeof(int) * camera.renderData.verticesCount/3);
        unsigned int *p =camera.renderData.matricesIndex+(camera.renderData.verticesCount- actualM.mesh.verticesCount)/3;
        for (int j = 0; j < actualM.mesh.verticesCount/3; j++)
        {
            p[j] = i - 1;
        }
    }
}
// o começo(nBlock,sizeBlock) = (nBlock)sizeBlock to (nBlock+1)sizeBlock -1
// 0 to  15 16 to 31
//  nblock0 nblock1
// v1x v1y v1z ... + v6x
// float *vertices =
