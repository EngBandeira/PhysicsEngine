
#version 450 core
layout (location = 0) in vec3 inColor;
layout (location = 0) out vec4 fragColor;
void main()
{
    // float d = sqrt(gl_PointCoord.x*gl_PointCoord.x + gl_PointCoord.y * gl_PointCoord.y)*1000;
    float d = gl_Po.x;
    fragColor = vec4(d,d,d,1.0);
}

