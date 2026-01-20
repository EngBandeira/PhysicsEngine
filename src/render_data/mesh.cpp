#include "mesh.hpp"
#include <X11/X.h>
#include <cstring>
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

#include "vendor/glad/glad.h"
#include <GLFW/glfw3.h>


#include "render_data.hpp"

// struct {
    VadiaMeshes* meshes;
    unsigned int meshesCount = 0,
                 verticesCount = 0,
                 verticesIndexCount = 0,
//                 *verticesIndex,
                *verticesIndexOffsets,//[0] = 0,[1] = v
                *meshObjects;
//     float *vertices;
// } compacted_meshes;

void RenderData::addMesh(MeshGenData genData) {
    Mesh mesh(genData);
    compacted_meshes.meshes = (VadiaGenMeshes*)realloc(compacted_meshes.meshes,++compacted_meshes.meshesCount * sizeof(VadiaMeshes));
    VadiaMeshes va {
        .verticesCount = mesh.verticesCount,
        .verticesIndexCount = mesh.verticesIndexCount
    };

    memcpy(compacted_meshes.meshes + compacted_meshes.meshesCount - 1, &va, sizeof(VadiaMeshes));

    compacted_meshes.meshObjects = (unsigned int*)realloc(compacted_meshes.meshObjects,compacted_meshes.meshesCount * sizeof(int));

    compacted_meshes.verticesIndexOffsets = (unsigned int*)realloc(compacted_meshes.verticesIndexOffsets,compacted_meshes.meshesCount * sizeof(int));
    compacted_meshes.verticesIndexOffsets[compacted_meshes.meshesCount-1] = mesh.verticesIndexCount;

    if(compacted_meshes.meshesCount > 1)
        compacted_meshes.verticesIndexOffsets[compacted_meshes.meshesCount-1] += compacted_meshes.verticesIndexOffsets[compacted_meshes.meshesCount-2];


    compacted_meshes.verticesCount += mesh.verticesCount;
    compacted_meshes.verticesIndexCount += mesh.verticesIndexCount;

    compacted_meshes.vertices = (float*)realloc(compacted_meshes.vertices,compacted_meshes.verticesCount * sizeof(float));
    memcpy(compacted_meshes.vertices - mesh.verticesCount, mesh.vertices, mesh.verticesCount);

    compacted_meshes.verticesIndex = (unsigned int*)realloc(compacted_meshes.verticesIndex,compacted_meshes.verticesIndexCount * sizeof(int));
    memcpy(compacted_meshes.verticesIndex - mesh.verticesIndexCount, mesh.verticesIndex, mesh.verticesIndexCount);
}

void RenderData::removeMesh(unsigned int index) {

}
