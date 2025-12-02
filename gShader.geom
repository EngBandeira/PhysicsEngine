
#version 460 core 
#pragma debug(on)
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
// layout (location = 1) out vec3 outColor;
out vec3 gColor;
out vec4 outValue;
out vec2 g_texCoord;
layout(std430, binding = 2) buffer TexCoordBuffer {
    vec2 texCoord[];
};
layout(std430, binding = 3) buffer TexIndexBuffer {
    uvec4 indexTexture[];
};
// layout (location = 1) in vec3 inColor;

void main(){
      //
      uvec4 a = indexTexture[gl_PrimitiveIDIn];

      gl_Position = gl_in[0].gl_Position;
      g_texCoord = texCoord[a.x];
      EmitVertex();

      gl_Position = gl_in[1].gl_Position;
      g_texCoord = texCoord[a.y];
      EmitVertex();

      gl_Position = gl_in[2].gl_Position;
      g_texCoord = texCoord[a.z];
      EmitVertex();

}
