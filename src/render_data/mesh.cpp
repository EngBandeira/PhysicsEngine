#include <X11/X.h>
#include <cstdlib>
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

#include "common.hpp"

#include "mesh.hpp"

#include "render_data.hpp"


unsigned int Render_Data::add_mesh(MeshGenData genData) {
    Mesh mesh(genData);

    for(unsigned int i = 0; i < mesh.index_count; i++) {
        mesh.vertices_index[i]  += compacted_meshes.vertices_count / 3;
        mesh.texture_vertices_index[i]  += compacted_meshes.texture_vertices_count / 2;
    }


    compacted_meshes.meshes = (Vadia_Mesh*)realloc(compacted_meshes.meshes,++compacted_meshes.meshes_count * sizeof(Vadia_Mesh));


    compacted_meshes.meshes[compacted_meshes.meshes_count - 1] = Vadia_Mesh();
    compacted_meshes.meshes[compacted_meshes.meshes_count - 1].vertices_count = mesh.vertices_count;
    compacted_meshes.meshes[compacted_meshes.meshes_count - 1].index_count = mesh.index_count;
    compacted_meshes.meshes[compacted_meshes.meshes_count - 1].texture_vertices_count = mesh.vertices_count;
    // compacted_meshes.mesh_objects = (unsigned int*)realloc(compacted_meshes.mesh_objects,compacted_meshes.meshes_count * sizeof(int));


    compacted_meshes.vertices_count += mesh.vertices_count;
    compacted_meshes.index_count += mesh.index_count;
    compacted_meshes.texture_vertices_count += mesh.texture_vertices_count;

    compacted_meshes.vertices = (float*)realloc(compacted_meshes.vertices,compacted_meshes.vertices_count * sizeof(float));
    memcpy(compacted_meshes.vertices + compacted_meshes.vertices_count - mesh.vertices_count, mesh.vertices, mesh.vertices_count * sizeof(float));

    compacted_meshes.vertices_index = (unsigned int*)realloc(compacted_meshes.vertices_index,compacted_meshes.index_count * sizeof(int));
    memcpy(compacted_meshes.vertices_index + compacted_meshes.index_count - mesh.index_count, mesh.vertices_index, mesh.index_count * sizeof(int));

    compacted_meshes.texture_vertices = (float*)realloc(compacted_meshes.texture_vertices,compacted_meshes.texture_vertices_count * sizeof(float));
    memcpy(compacted_meshes.texture_vertices + compacted_meshes.texture_vertices_count - mesh.texture_vertices_count, mesh.texture_vertices, mesh.texture_vertices_count * sizeof(float));

    compacted_meshes.texture_vertices_index = (unsigned int*)realloc(compacted_meshes.texture_vertices_index,compacted_meshes.index_count * sizeof(int));
    memcpy(compacted_meshes.texture_vertices_index + compacted_meshes.index_count - mesh.index_count, mesh.texture_vertices_index, mesh.index_count * sizeof(int));


    compacted_meshes.vertices_index_offsets = (unsigned int*)realloc(compacted_meshes.vertices_index_offsets,
                                                        sizeof(int) * (compacted_meshes.meshes_count+ 1));
    compacted_meshes.vertices_index_offsets[compacted_meshes.meshes_count] =
            compacted_meshes.vertices_index_offsets[compacted_meshes.meshes_count - 1] + mesh.index_count;

    compacted_meshes.texture_vertices_index_offsets = (unsigned int*)realloc(compacted_meshes.texture_vertices_index_offsets,
            sizeof(int) * (compacted_meshes.meshes_count+ 1));
    compacted_meshes.texture_vertices_index_offsets[compacted_meshes.meshes_count] =
            compacted_meshes.texture_vertices_index_offsets[compacted_meshes.meshes_count - 1] + mesh.index_count;


    glBindBuffer(GL_SHADER_STORAGE_BUFFER,ssbos[SSBOS::TextureVerticesSSBO]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * compacted_meshes.texture_vertices_count,
                        compacted_meshes.texture_vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);


    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(float) * compacted_meshes.vertices_count,
                        compacted_meshes.vertices,GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    mesh.deleteMesh();
    return compacted_meshes.meshes_count - 1;
}

void Render_Data::remove_mesh(unsigned int index) {

}
