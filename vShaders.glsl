
#version 450 core
layout (location = 0) in vec4 position;
layout (location = 0) out vec3 outColor;
uniform int caseta;
void main()
{
   gl_Position = position;
   vec3 colors[3] = vec3[3](
      vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));
   outColor = colors[gl_VertexID];
}