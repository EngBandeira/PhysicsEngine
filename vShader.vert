#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in uint modelMxIndex;
// layout (location = 0) out vec3 outColor;
// out vec4 outValue;
// layout(std430, binding = 0) buffer tex {
//     vec2 texCoord[];
// };
// layout(std430, binding = 1) buffer texIn {
//     int texIndex[];
// };
layout(std430, binding = 2) buffer modelMxBuffer {
    mat4 modelsMatrix[];
};

out uint modelIndexGeom;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = (projection * view * modelsMatrix[modelMxIndex] * vec4(position, 1));

    modelIndexGeom = modelMxIndex;
}
