#version 460 core
layout(location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 modelMatrix;
// uniform mvp;
void main()
{
    mat4 mvp = projection * view  * modelMatrix;
    gl_Position = (mvp * vec4(position, 1));
}
