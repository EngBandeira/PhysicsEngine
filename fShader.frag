#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable
// layout (location = 0) in vec3 inColor;
layout(location = 0) out vec4 fragColor;
uniform sampler2D Texture; //This is texture 38
//  in vec3 vColor;
in vec2 g_texCoord;
in vec3 gColor;
in vec3 norm;
in flat uint modelIndexFrag;
uniform mat4 model;
uniform vec3 viewVec;

uniform sampler2DArray textures[16]; //64x64, 4096x4096
// uniform sampler2DArray textures; //64x64, 4096x4096

layout(std430, binding = 3) buffer textureIndxBuffer {
    uvec2 textureInxBuffer[]; //which tex array, index
};
layout(std430, binding = 6) buffer renderFlagBuffer {
    int renderFlagsBuffer[]; //which tex array, index//abcd efgh
    //                h:have texture
};

// out vec4 outValue;

void main()
{
    // float d = sqrt(gl_PointCoord.x*gl_PointCoord.x + gl_PointCoord.y * gl_PointCoord.y)*1000;
    // float d = gl_Position.x;
    // fragColor = vec4(d,d,d,1.0);
    // fragColor = vec4(inColor,1);
    // fragColor = vec4(vColor,1);
    // fragColor = vec4(gColor,1);
    //
    int pinto = gl_PrimitiveID % 3;
    switch (pinto) {
        case 0:
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        break;
        case 1:
        fragColor = vec4(0.0, 1.0, 0.0, 1.0);

        break;
        case 2:
        fragColor = vec4(0.0, 0.0, 1.0, 1.0);

        break;
    }
    //
    //
    fragColor = vec4(1, 0, 0, 1);
    if (bool(renderFlagsBuffer[modelIndexFrag] & 1))
    {
        fragColor = texture(textures[textureInxBuffer[modelIndexFrag].x],
                vec3(g_texCoord, textureInxBuffer[modelIndexFrag].y));
    }

    // fragColor += vec4(1.0, 0.0, 0.0, 1.0) * gl_FragCoord.w * 0.3;
    // fragColor = texture(textures,
    // vec3(g_texCoord, 0));
    // vec4 preta = model * vec4(normalVecs[NormalVecIndex[gl_PrimitiveID]], 1);
    // vec3 bingoludo = normalize(preta.xyz);
    // float pinto = dot(norm, viewVec);
}
