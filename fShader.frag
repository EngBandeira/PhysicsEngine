
#version 460 core 
// layout (location = 0) in vec3 inColor;
layout (location = 0) out vec4 fragColor;
// uniform sampler2D Texture;
//  in vec3 vColor;
// in vec2 v_texCoord;
void main()
{

    // float d = sqrt(gl_PointCoord.x*gl_PointCoord.x + gl_PointCoord.y * gl_PointCoord.y)*1000;
    // float d = gl_Position.x;
    // fragColor = vec4(d,d,d,1.0);     
    // fragColor = vec4(inColor,1);
    // fragColor = vec4(vColor,1);
    fragColor = vec4(1,0,0,1);

    // fragColor = texture(Texture,v_texCoord);
    // fragColor = vec4(inColor,1);
}

