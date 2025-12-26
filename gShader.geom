
#version 460 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
out vec4 outValue;
out vec2 g_texCoord;
out vec3 normalVec;
out vec3 normalVecT;
out flat uint modelIndexFrag;
out noperspective vec4 worldPos;
out noperspective vec4 worldPosT;

in uint modelIndexGeom[3];

out vec4 transformFeedback;

uniform mat4 projection;
uniform mat4 view;

layout(std430, binding = 2) buffer modelMxBuffer {
    mat4 modelsMatrix[];
};
layout(std430, binding = 4) buffer TexCoordBuffer {
    vec2 texCoord[];
};
layout(std430, binding = 5) buffer TexIndexBuffer {
    uvec3 indexTexture[];
};
layout(std430, binding = 7) buffer NormalVecsBuffer {
    float normalVecsBuffer[]; // to getway with vec3 limitations
};
layout(std430, binding = 8) buffer NormalVecsIndexBuffer {
    uint normalVecsIndexBuffer[];
};

void main() {
    //
    vec4 niopaVec = vec4(normalVecsBuffer[3 * normalVecsIndexBuffer[gl_PrimitiveIDIn]],
            normalVecsBuffer[3 * normalVecsIndexBuffer[gl_PrimitiveIDIn] + 1],
            normalVecsBuffer[3 * normalVecsIndexBuffer[gl_PrimitiveIDIn] + 2],0);

    vec4 posito[3] = {
        (inverse(projection)*gl_in[0].gl_Position),
        (inverse(projection)*gl_in[1].gl_Position),
        (inverse(projection)*gl_in[2].gl_Position)
        };
    vec4 positoT[3] = {
        (inverse(projection)*gl_in[2].gl_Position),
        (inverse(projection)*gl_in[2].gl_Position),
        (inverse(projection)*gl_in[2].gl_Position)
    };
    vec4 a = posito[0] - posito[1]; //need to try with four dimensions
    vec4 b = posito[2] - posito[1]; //need to try with four dimensions
    normalVec = normalize(cross(a.xyz, b.xyz));

    vec4 aT = positoT[0] - positoT[1]; //need to try with four dimensions
    vec4 bT = positoT[2] - positoT[1]; //need to try with four dimensions
    normalVecT = normalize(cross(aT.xyz, bT.xyz));

    // mat3 pica = mat3(inverse(transpose(view * modelsMatrix[modelIndexGeom[1]])));

    // normalVecT = normalize(pica * niopaVec);
    modelIndexFrag = modelIndexGeom[1];
    gl_PrimitiveID = gl_PrimitiveIDIn;
    transformFeedback = vec4(normalVec - normalVecT, 1);

    uvec3 inxTex = indexTexture[gl_PrimitiveIDIn];
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        worldPos = posito[i];
        worldPosT = positoT[i];
        g_texCoord = texCoord[inxTex[i]];
        EmitVertex();
    }

    EndPrimitive();
}
