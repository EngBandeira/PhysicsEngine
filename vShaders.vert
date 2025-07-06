
#version 450 core
layout (location = 0) in vec4 position;
layout (location = 0) out vec3 outColor;
out vec4 outValue;
uniform int caseta;
uniform mat4 projection;
uniform mat4 view;
void main()
{
// m v p
   // gl_Position = projection *position;
   gl_Position = (projection * view * position);
   // gl_Position = projection * position;
   // outValue = vec4(gl_Position.xyz,gl_VertexID);
   // outValue = gl_Position;
   outValue = view[3];
   vec3 colors[3] = vec3[3](
      vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));
   outColor = colors[gl_VertexID];
}