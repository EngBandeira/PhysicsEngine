#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in uint modelMxIndex;

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
