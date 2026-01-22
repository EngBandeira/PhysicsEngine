#include <X11/X.h>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <string.h>

#include "mesh.hpp"

template <typename T>
T *exportvec(unsigned int *count, std::vector<T> vec) {
    *count = vec.size();
    T *rt = (T *) malloc(sizeof(T) * (*count));
    memcpy(rt,vec.data(),sizeof(T) * (*count));
    return rt;
}

Mesh::Mesh(MeshGenData genData): meshPath(genData.path) { Mesh::sanatize(); }
Mesh::~Mesh() {}

void Mesh::deleteMesh() {
    free(vertices);
    free(texture_vertices);
    free(normal_vertices);
    free(vertices_index);
    free(texture_vertices_index);
    free(normal_vertices_index);
    free(mtl);
    for( unsigned int i = 0; i < oCount; i++ ) free(o[i]);
    free(o);
}
