#pragma once

#include "model.hpp"
#include "log.hpp"
#include "shader.hpp"
#include <GLFW/glfw3.h>
#include <vector>


#define TEXTURE_HANDLERS_COUNT 7
#define SSBO_COUNT 7
#define VBO_COUNT 2
#define SCR_X 1920
#define SCR_Y 1080
#define MAX_FILE_EXTENSION_LENGHT 6
#define RAW_TEX_EXTENSION "raw"
#define RAW_TEX_EXTENSION_LENGHT 3

void sendWarning(const char *msg);

void send(const char *msg);

void sendError(const char *msg);

template<typename T, size_t N>
bool matchPairs(T *buffer, int j, const T (&pairs)[N]) {
    for(size_t i = 0; i < N; i++) {
        if(buffer[i+j] != pairs[i]) return false;
    }
    return true;
}


enum materialType{
    SOLID_COLOR,
    TEXTURE
};

struct TextureHandler {
    unsigned int texture,texDimensions, texturesCount = 0,emptyTexturesCount;
    unsigned int *materialIndex,*emptyTextures,*texUtilitary;
    unsigned int addTex(unsigned char *localBuffer);
    void rmTex(unsigned int index);
    TextureHandler(){};
};
struct TextureLocation{
    unsigned int handler, index;
};

class Material {
    public:
    float K[3*3];//Ka Kd Ks
    float Ni, d,bm;
    TextureLocation maps[4];// Ka Kd Ks Normal
    enum materialType type;
};

enum SSBOS{
    ModelMatricesSSBO,
    TextureCoordSSBO,
    TextureIndexSSBO,
    MaterialsSSBO,
    ModelsMaterialsSSBO,
    NormalVecsSSBO,
    NormalVecsIndexSSBO
};

class RenderData {
    public:
    TextureHandler textureHandlers[TEXTURE_HANDLERS_COUNT];
    unsigned int texUtilitary;
    Material *materials;
    unsigned int materialsCount = 0;
    std::vector<Model> models;
    TextureLocation addTexToHandler(char *path,bool toProcess=1);
    struct MeshRenderData {
        unsigned int meshesCount = 0;
        unsigned int  verticesCount = 0, verticesIndexCount = 0, textureVerticesCount=0,
                            textureVerticesIndexCount=0,normalVerticesCount=0,// normalIndexCount = nOfPrimitives = verticesIndexCount /3
                            verticesIndexOffset= 0,textureIndexOffset = 0,normalIndexOffset=0;
        float *vertices,*matrices,*textureVertices,*normalVertices;
        unsigned int *verticesIndex,*modelParent,*textureVerticesIndex,*normalVerticesIndex,*modelMaterial;
        // modelIndex: witch
        //count(matricesIndex) = count(vertices) = verticesCount
    } mesh;



    RenderData();
    glm::mat4 *getNMatrix(unsigned short index);
    void freeRenderData();
};

class Camera {
    public:
    glm::vec4 *up,*foward,*right;
    glm::vec3 angle;
    glm::mat4 translation, rotation, localTranslation, viewMatrix, projMatrix;
    glm::vec4 getPosition();
    glm::vec4 getUp();
    glm::vec4 getFoward();
    glm::vec4 getRight();
    Camera();
};


struct MaterialGenData{
    glm::vec3 K[3];//Ka Kd Ks
    float Ni, d,bm;
    char *maps[4];// Ka Kd Ks Normal
    enum materialType type;
};

struct ModelGenData {
    MeshGenData mesh;
    unsigned int materialIndex;
};



class Render {
    public:
    unsigned int ssbos[SSBO_COUNT];
    unsigned int flags; // abcd efgh: h = Update renderData
    unsigned int EBO, FBO_FROM,FBO_TO,RBO, TBO, QUERY,
                 texToRenderOver,texToShowFrom;
    unsigned int feedbacksize,feedbacknumber,samples=4;
    unsigned int VBOS[VBO_COUNT], VAO;

    Camera camera;
    bool transFeed = false;

    void start(void(*op1)(),void(*op2)(),void(*op3)());

    unsigned int allocMaterial(MaterialGenData data);
    void freeMaterial(unsigned int index);

    unsigned int pushModels(MeshGenData mesh,unsigned int materialIndex);//Pass a mesh and a material to the pipeline
    unsigned int pushModels(MeshGenData mesh,MaterialGenData material);
    unsigned int pushModels(MeshGenData mesh);//Pass a mesh and a material to the pipeline
    void popModels(unsigned int index);


    Render(GLFWwindow *win);
    ~Render();
    private:

    int minFramesToUp = -1;


    RenderData renderData;
    unsigned short selectedModelIndex=0;
    unsigned int shaderProgram;
    Shader shader;
    GLFWwindow *glfwWin;

    void updatePipeline();
    void once();
    void runTime();
    void newframe();
    void renderDrawing();
    void imguiSetting();
    void input();

};
