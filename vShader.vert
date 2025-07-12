
#version 460 core 
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 txPosition;
// layout (location = 0) out vec3 outColor;
// layout(std430, binding = 3) buffer tex{
//    vec2 texCoord[];
// };
// layout(std430, binding = 3) buffer texIn{
//    int texIndex[];
// };
// out vec3 vColor;

// uniform int caseta;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
   // m v p
   // gl_Position = projection *position;
   gl_Position = (projection * view * model* position);
   // gl_Position = projection * position;
   // outValue = vec4(gl_Position.xyz,gl_VertexID);
   // outValue = gl_Position;
   // outValue = vec4(gl_PrimitiveID);
   vec3 colors[3] = vec3[3](
      vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));
   // vColor = colors[gl_VertexID];

   //   vec3 colors[6] = vec3[6](
      // vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0),
      // vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));
}

