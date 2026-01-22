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


void RenderData::addMesh(MeshGenData genData) {
    Mesh mesh(genData);
    compacted_meshes.meshes = (VadiaMeshes*)realloc(compacted_meshes.meshes,++compacted_meshes.meshes_count * sizeof(VadiaMeshes));

    compacted_meshes.meshes[compacted_meshes.meshes_count - 1].verticesCount = mesh.vertices_count;
    compacted_meshes.meshes[compacted_meshes.meshes_count - 1].verticesIndexCount = mesh.vertices_index_count;

    compacted_meshes.mesh_objects = (unsigned int*)realloc(compacted_meshes.mesh_objects,compacted_meshes.meshes_count * sizeof(int));

    compacted_meshes.vertices_index_offsets = (unsigned int*)realloc(compacted_meshes.vertices_index_offsets,(compacted_meshes.meshes_count + 1) * sizeof(int));
    compacted_meshes.vertices_index_offsets[compacted_meshes.meshes_count] = mesh.vertices_index_count + compacted_meshes.vertices_index_offsets[compacted_meshes.meshes_count-1];


    compacted_meshes.vertices_offsets = (unsigned int*)realloc(compacted_meshes.vertices_offsets,(compacted_meshes.meshes_count + 1) * sizeof(int));
    compacted_meshes.vertices_offsets[compacted_meshes.meshes_count] = mesh.vertices_count + compacted_meshes.vertices_offsets[compacted_meshes.meshes_count-1];


    compacted_meshes.texture_vertices_index_offsets = (unsigned int*)realloc(compacted_meshes.texture_vertices_index_offsets,(compacted_meshes.meshes_count + 1) * sizeof(int));
    compacted_meshes.texture_vertices_index_offsets[compacted_meshes.meshes_count] = mesh.texture_vertices_index_count + compacted_meshes.texture_vertices_index_offsets[compacted_meshes.meshes_count-1];


    compacted_meshes.texture_vertices_offsets = (unsigned int*)realloc(compacted_meshes.texture_vertices_offsets,(compacted_meshes.meshes_count + 1) * sizeof(int));
    compacted_meshes.texture_vertices_offsets[compacted_meshes.meshes_count] = mesh.texture_vertices_count/2 + compacted_meshes.texture_vertices_offsets[compacted_meshes.meshes_count-1];



    compacted_meshes.vertices_count += mesh.vertices_count;
    compacted_meshes.vertices_index_count += mesh.vertices_index_count;

    compacted_meshes.texture_vertices_count += mesh.texture_vertices_count;
    compacted_meshes.texture_vertices_index_count += mesh.texture_vertices_index_count;

    compacted_meshes.vertices = (float*)realloc(compacted_meshes.vertices,compacted_meshes.vertices_count * sizeof(float));
    memcpy(compacted_meshes.vertices + compacted_meshes.vertices_count - mesh.vertices_count, mesh.vertices, mesh.vertices_count * sizeof(float));

    compacted_meshes.vertices_index = (unsigned int*)realloc(compacted_meshes.vertices_index,compacted_meshes.vertices_index_count * sizeof(int));
    memcpy(compacted_meshes.vertices_index + compacted_meshes.vertices_index_count - mesh.vertices_index_count, mesh.vertices_index, mesh.vertices_index_count * sizeof(int));


    compacted_meshes.texture_vertices = (float*)realloc(compacted_meshes.texture_vertices,compacted_meshes.texture_vertices_count * sizeof(float));
    memcpy(compacted_meshes.texture_vertices + compacted_meshes.texture_vertices_count - mesh.texture_vertices_count, mesh.texture_vertices, mesh.texture_vertices_count * sizeof(float));

    compacted_meshes.texture_vertices_index = (unsigned int*)realloc(compacted_meshes.texture_vertices_index,compacted_meshes.texture_vertices_index_count * sizeof(int));
    memcpy(compacted_meshes.texture_vertices_index + compacted_meshes.texture_vertices_index_count - mesh.texture_vertices_index_count, mesh.texture_vertices_index, mesh.texture_vertices_index_count * sizeof(int));

    compacted_meshes.mesh_objects[compacted_meshes.meshes_count-1] = -1;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER,ssbos[SSBOS::TextureVerticesSSBO]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                    sizeof(float) * compacted_meshes.texture_vertices_count,
                    compacted_meshes.texture_vertices,GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER,ssbos[SSBOS::TextureIndexSSBO]);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                    sizeof(unsigned int) * compacted_meshes.texture_vertices_index_count,
                    compacted_meshes.texture_vertices_index,GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(unsigned int) * compacted_meshes.vertices_index_count,
                    compacted_meshes.vertices_index,GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void RenderData::removeMesh(unsigned int index) {

}
