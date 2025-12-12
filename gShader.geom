
#version 460 core
#pragma debug(on)
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
// layout (location = 1) out vec3 outColor;
out vec3 gColor;
out vec4 outValue;
out vec2 g_texCoord;
out vec3 norm;

// layout (location = 1) in vec3 inColor;
in uint belopinto[3];

uniform mat4 projection;
uniform mat4 view;
// uniform mat4 model;

void main() {
    //
    // uvec4 a = indexTexture[gl_PrimitiveIDIn];

    gl_Position = gl_in[0].gl_Position;
    // g_texCoord = texCoord[a.x];
    gl_PrimitiveID = gl_PrimitiveIDIn;
    outValue = vec4(float(belopinto[0]), float(belopinto[1]), float(belopinto[2]), 1);
    // outValue = modelsMatrix[modelsMatrixIndex[gl_PrimitiveIDIn]] * vec4(1, 0, 0, 0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    // g_texCoord = texCoord[a.y];
    gl_PrimitiveID = gl_PrimitiveIDIn;
    outValue = vec4(float(belopinto[0]), float(belopinto[1]), float(belopinto[2]), 1);

    // outValue = modelsMatrix[modelsMatrixIndex[gl_PrimitiveIDIn]] * vec4(0, 1, 0, 0);
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    // g_texCoord = texCoord[a.z];
    gl_PrimitiveID = gl_PrimitiveIDIn;
    outValue = vec4(float(belopinto[0]), float(belopinto[1]), float(belopinto[2]), 1);

    // outValue = modelsMatrix[modelsMatrixIndex[gl_PrimitiveIDIn]] * vec4(1, 0, 1, 0);
    EmitVertex();
    EndPrimitive();
}
