
#version 460 core 

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
// layout (location = 1) out vec3 outColor;
// layout (location = 1) in vec3 inColor;

void main(){
      gl_Position = gl_in[0].gl_Position;
      EmitVertex();
      gl_Position = gl_in[1].gl_Position;
      EmitVertex();
      gl_Position = gl_in[2].gl_Position;
      EmitVertex();
      // EndPrimitive();
//      vec3 colors[6] = vec3[6](
      // vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0),
      // vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));
      
      // outColor = colors[gl_PrimitiveID];
      // outColor = inColor;
}
