#pragma once


struct MeshGenData {
    char *path;
};


class Mesh {
    void obj();
    void pe();
    public:
    unsigned int oCount = 0;
    char *mtl;
    char **o;

    unsigned int vertices_count = 0,
                 vertices_index_count = 0,
                 texture_vertices_count = 0,
                 texture_vertices_index_count = 0,
                 normal_vertices_count = 0,
                 vertices_offset_index = 0, // normalIndexCount = nOfPrimitives = verticesIndexCount /3

                // pointers
                *vertices_index,
                *texture_vertices_index,
                *normal_vertices_index;

    float *vertices,
          *texture_vertices,
          *normal_vertices;

    const char *meshPath;

    void deleteMesh();
    Mesh() = default;
    Mesh(MeshGenData genData);
    void sanatize();
    ~Mesh();
};
