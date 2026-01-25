#version 460 core
layout(location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
layout(std430, binding = 2) buffer MatricesSSBO {
    mat4 matrices_ssbo[];
};
layout(std430, binding = 3) buffer VerticesOffsetSSBO {
    uint vertices_offset_ssbo[];
};
uniform uint objects_count;
// uniform mvp;
void main()
{
    int i = 0;
    for(; i < objects_count; i++){
        if(vertices_offset_ssbo[i] > gl_VertexID)
            break;
    }
    i--;
    mat4 mvp = projection * view  * matrices_ssbo[i];
    gl_Position = (mvp * vec4(position, 1));
}
