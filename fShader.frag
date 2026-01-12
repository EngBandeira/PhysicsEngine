#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 fragColor;

struct TextureLocation {
    int handler;
    uint index;
};

const uint MATERIAL_SOLID_COLOR = 0u;
const uint MATERIAL_TEXTURE = 1u;

const uint MATERIAL_MAPS_KA = 0u;
const uint MATERIAL_MAPS_KD = 1u;
const uint MATERIAL_MAPS_KS = 2u;
const uint MATERIAL_MAPS_NORMAL = 3u;

struct Material {
    float K[9];
    float Ni, d, bm;
    TextureLocation maps[4];
    uint type;
};

struct Lamp {
    vec4 position;
};

layout(std430, binding = 7) buffer LampsBuffer {
    Lamp lamps[];
};

uniform vec4 camPosition;
uniform int lampsCount;
in flat uint modelParentFrag;
in flat Material material;
in vec2 g_texCoord;
in vec3 normalVec;
in vec4 worldPos;

uniform mat4 view;

uniform int normalATIVO;
uniform float normalV;
uniform sampler2DArray textures[16];

void main() {
    fragColor = vec4(1, 0, 0, 1);
    if (material.type == MATERIAL_TEXTURE) {
        fragColor = texture(textures[material.maps[MATERIAL_MAPS_KD].handler],
                vec3(g_texCoord, material.maps[MATERIAL_MAPS_KD].index));
    }
    else {
        fragColor = vec4(1, 0, 0, 1);
    }
    float lightIntensity = 1;
    float illumA = 0;
    float alpha = 2;
    float illumD = 0;
    float illumS = 0;
    vec3 V = camPosition.xyz - worldPos.xyz; //View
    float viewLenght = length(V);
    V = V / viewLenght;
    vec3 N = normalVec.xyz;

    if(material.maps[MATERIAL_MAPS_NORMAL].handler >= 0) {
        vec4 normalColor = texture(textures[material.maps[MATERIAL_MAPS_NORMAL].handler],
                vec3(g_texCoord, material.maps[MATERIAL_MAPS_NORMAL].index));
        normalColor.y *= -1.0f;
        N += normalize(normalColor.xzy) * normalV;
    }

    for (int i = 0; i < lampsCount; i++) {
        vec3 L = worldPos.xyz - lamps[i].position.xyz; //Incident
        float lNorm  = length(L);
        L = L / lNorm;
        vec3 R = reflect(L, N); //Reflected

        //Difuse
        illumD += 1 * max(dot(L, N), 0) / (lNorm + viewLenght);
        // Specular
        illumS += 1 * max(pow(dot(V, R), alpha), 0) / (lNorm + viewLenght);
    }
    fragColor = fragColor * (illumA + illumD + illumS);
    fragColor.w = 1;
}
