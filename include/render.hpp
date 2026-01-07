#pragma once

#include "model.hpp"
#include "log.hpp"
#include "shader.hpp"
#include <GLFW/glfw3.h>
#include <asm-generic/errno.h>
#include <vector>

#define TEXTURE_HANDLERS_COUNT 7
#define LAYERS_COUNT 2
#define SSBO_PER_LAYER_COUNT 6
#define VBO_COUNT 2
#define SCR_X 1920
#define SCR_Y 1080
#define MAX_FILE_EXTENSION_LENGHT 6
#define RAW_TEX_EXTENSION "raw"
#define RAW_TEX_EXTENSION_LENGHT 3

void sendWarning(const char *msg);

void send(const char *msg);

void sendError(const char *msg);


enum LAYER{
    SPECIAL_LAYER,
    COMMON_LAYER
};

enum materialType {
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
struct TextureLocation {
    unsigned int handler, index;
};

class Material {
    public:
    float K[3*3];//Ka Kd Ks
    float Ni, d,bm;
    TextureLocation maps[4];// Ka Kd Ks Normal
    enum materialType type;
};

enum SSBOS {
    ModelMatricesSSBO ,
    TextureCoordSSBO,
    TextureIndexSSBO,
    ModelsMaterialsSSBO,
    NormalVecsSSBO,
    NormalVecsIndexSSBO,
    MaterialsSSBO
};

struct MeshRenderData {
    unsigned int ssbos[SSBO_PER_LAYER_COUNT];
    std::vector<Model> models;
    unsigned int ebo,vbos[VBO_COUNT], vao;
    unsigned int meshesCount = 0;
    unsigned int  verticesCount = 0, verticesIndexCount = 0, textureVerticesCount=0,
                        textureVerticesIndexCount=0,normalVerticesCount=0,// normalIndexCount = nOfPrimitives = verticesIndexCount /3
                        verticesIndexOffset= 0,textureIndexOffset = 0,normalIndexOffset=0;
    float *vertices,*matrices,*textureVertices,*normalVertices;
    unsigned int *verticesIndex,*modelParent,*textureVerticesIndex,*normalVerticesIndex,*modelMaterial;
    void init();
    void freeData();
};

class RenderData {
    public:
    glm::vec4 getPositionOfModel(unsigned short index,unsigned int layerIndex);
    unsigned int materialsSSBO;
    TextureHandler textureHandlers[TEXTURE_HANDLERS_COUNT];
    unsigned int texUtilitary;
    Material *materials;
    unsigned int materialsCount = 0;

    TextureLocation addTexToHandler(char *path,bool toProcess=1);
    MeshRenderData LayersData[LAYERS_COUNT];



    RenderData();
    void scaleModel(glm::vec3 scale, unsigned short index,unsigned int layerIndex);
    void scaleModel(float scale, unsigned short index,unsigned int layerIndex);
    void translateModel(glm::vec3 translation, unsigned short index,unsigned int layerIndex);
    void rotateModel(float angl, AXIS axis, unsigned short index,unsigned int layerIndex);
    void positionateModel(glm::vec3 position, unsigned short index,unsigned int layerIndex);
    glm::mat4 *getNMatrix(unsigned short index,unsigned int layerIndex);

    void freeData();
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
    MaterialGenData();
};

struct ModelGenData {
    MeshGenData mesh;
    unsigned int materialIndex;
};

enum FILE_TYPES{
    COMMON_FILE,
    FOLDER_FILE,
    MESH_FILE,
    IMAGE_FILE,
    CODE_FILE
};

struct File{
    char *simpleName; // complete name = simpleName + '.' + extension
    char *extension;
    char *completeName;
    FILE_TYPES type = FILE_TYPES::COMMON_FILE;
};

struct Assets{
    std::vector<File> files;
    unsigned int framesDelay = 120;
    char *directory = nullptr;
    void update();
};


class Render {
    public:
    unsigned int FBO_FROM, FBO_TO, RBO, TBO, QUERY,
                 texToRenderOver,texToShowFrom;
    unsigned int feedbacksize,feedbacknumber,samples=4;

    Camera camera;
    bool transFeed = false;

    void start(void(*op1)(),void(*op2)(),void(*op3)());

    unsigned int allocMaterial(MaterialGenData data);
    void freeMaterial(unsigned int index);

    unsigned int pushModels(MeshGenData mesh,unsigned int materialIndex, unsigned int layerIndex);//Pass a mesh and a material to the pipeline
    unsigned int pushModels(MeshGenData mesh,MaterialGenData material, unsigned int layerIndex);
    unsigned int pushModels(MeshGenData mesh, unsigned int layerIndex);//Pass a mesh and a material to the pipeline
    void popModels(unsigned int index,unsigned int layerIndex);


    Render(GLFWwindow *win);
    ~Render();
    private:

    int minFramesToUp = -1;
    unsigned int assetsDelayCounter = 0;

    Assets assets;
    RenderData renderData;
    unsigned short selectedModelIndex=0;
    unsigned int shaderProgram;
    Shader shader;
    GLFWwindow *glfwWin;

    unsigned int setAVector(glm::vec3 positon,glm::vec3 direction);

    void updatePipeline(unsigned int layerIndex);
    void once();

    void update();
    void newframe();
    void renderDrawing(unsigned int layerIndex);
    void imguiSetting();
    void input();

};
