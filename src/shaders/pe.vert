#version 460 core
layout(location = 0) in vec3 position;

layout(std430, binding = 2) buffer MatricesSSBO {
    mat4 matrices_ssbo[];
};
layout(std430, binding = 3) buffer VerticesOffsetSSBO {
    uint vertices_offset_ssbo[];
};
uniform uint objects_count;

uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position = projection * view * vec4(position, 1);
}
