#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in uint modelParent;

layout(std430, binding = 0) buffer modelMxBuffer {
    mat4 modelsMatrix[];
};

out uint modelParentGeom;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = (projection * view * modelsMatrix[modelParent] * vec4(position, 1));

    modelParentGeom = modelParent;
}
