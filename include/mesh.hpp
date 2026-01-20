#pragma once


struct MeshGenData {
    char *path;
};


class Mesh {
    public:
    unsigned int oCount = 0;
    char *mtl;
    char **o;

    unsigned int verticesCount = 0,
                 verticesIndexCount = 0,
                 textureVerticesCount = 0,
                 textureVerticesIndexCount = 0,
                 normalVerticesCount = 0,
                 verticesOffsetIndex = 0, // normalIndexCount = nOfPrimitives = verticesIndexCount /3

                // pointers
                *verticesIndex,
                *textureVerticesIndex,
                *normalVerticesIndex;

    float *vertices,
          *textureVertices,
          *normalVertices;

    const char *meshPath;

    void deleteMesh();
    Mesh() = default;
    Mesh(MeshGenData genData);
    void sanatize();
    ~Mesh();
};
