
#version 450 core
layout (location = 0) in vec4 position;
layout (location = 0) out vec3 outColor;
out vec4 outValue;
uniform int caseta;
uniform mat4 projection;
void main()
{
   // ALWAYS USE vec4 W != 0
   // float Cy = 1;
   // float ta = tan(3.14/6);
   // float beta = Cy/(ta*position.z);
   // gl_Position = vec4(position.x/position.z ,position.y/position.z, 1,position.z); 
   // gl_Position = vec4((projection * position).xyz,1.0);
   gl_Position = (projection * position);

   outValue = gl_Position;
   vec3 colors[3] = vec3[3](
      vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));
   outColor = colors[gl_VertexID];
}