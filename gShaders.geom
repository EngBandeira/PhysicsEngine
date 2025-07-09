
#version 460 core 

layout (location = 0) out vec3 outColor;

void main(){
     vec3 colors[6] = vec3[6](
      vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0),
      vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));
      outColor = colors[gl_PrimitiveID];
}
