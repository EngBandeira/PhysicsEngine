#version 460 core
#pragma debug(on)
layout(location = 0) in vec3 position;
layout(location = 1) in uint modelMxIndex;
// layout (location = 0) out vec3 outColor;
// out vec4 outValue;
// layout(std430, binding = 0) buffer tex {
//     vec2 texCoord[];
// };
// layout(std430, binding = 1) buffer texIn {
//     int texIndex[];
// };
layout(std430, binding = 2) buffer modelMxBuffer {
    mat4 modelsMatrix[];
};

out uint belopinto;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    // m v p
    // gl_Position = projection *position;
    gl_Position = (projection * view * modelsMatrix[modelMxIndex] * vec4(position, 1));
    // gl_Position = projection * position;
    // outValue = vec4(gl_Position.xyz, 1);
    // outValue = gl_Position;
    // outValue = vec4(gl_PrimitiveID);
    belopinto = modelMxIndex;
    vec3 colors[3] = vec3[3](
            vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));
    // vColor = colors[gl_VertexID];

    //   vec3 colors[6] = vec3[6](
    // vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0),
    // vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(0.0, 0.0, 1.0));
}
