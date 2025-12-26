#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 fragColor;

in vec2 g_texCoord;
in vec3 gColor;
in vec3 normalVec;
in vec3 normalVecT;
in flat uint modelIndexFrag;
in noperspective vec4 worldPos;
in noperspective vec4 worldPosT;

uniform vec4 cameraPosition;
uniform vec4 lampPosition;
// uniform vec4 cameraPositionT;
uniform mat4 projection;
uniform mat4 view;
uniform sampler2DArray textures[16];

uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float lightIntensity;
uniform float alpha;

uniform int test;
layout(std430, binding = 3) buffer textureIndxBuffer {
    uvec2 textureInxBuffer[]; //which tex array, index
};
layout(std430, binding = 6) buffer renderFlagBuffer {
    int renderFlagsBuffer[]; //which tex array, index//abcd efgh
    //h:have texture
};

void main()
{
    vec3 L = normalize(worldPos.xyz - lampPosition.xyz); //Incident
    vec3 V = normalize(cameraPosition.xyz - worldPos.xyz); //View
    vec3 R = reflect(L, normalVec); //Reflected
    vec3 N = normalVec.xyz;
    //Ambient
    float illumA = Ka * lightIntensity;
    //Difuse
    float illumD = Kd * max(dot(L, N), 0);
    // Specular
    float illumS = Ks * max(pow(dot(V, R), alpha), 0);

    fragColor = vec4(1, 0, 0, 1);
    if (bool(renderFlagsBuffer[modelIndexFrag] & 1))
    {
        fragColor = texture(textures[textureInxBuffer[modelIndexFrag].x],
                vec3(g_texCoord, textureInxBuffer[modelIndexFrag].y));
    }
    fragColor = fragColor * (illumA + illumD + illumS);
    fragColor.w = 1;
}
