#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in uint modelParent;

layout(std430, binding = 0) buffer modelMxBuffer {
    mat4 modelsMatrix[];
};
layout(std430, binding = 7) buffer ModelFlagsBuffer {
    uint models_flags[];
};
out uint modelParentGeom;
uniform mat4 projection;
uniform mat4 view;
uniform int vertexCase;

uniform mat4 lamp_projection;
uniform mat4 lamp_view;

void main()
{
    mat4 mvp;
    switch(vertexCase ){
        case 0: {
            if(models_flags[modelParent] == 0){
                mvp = projection * view  * modelsMatrix[modelParent];
            }
            else if(models_flags[modelParent] == 1){
                mvp = projection  * modelsMatrix[modelParent];
            }
            break;
        }
        case 1: {
            mvp = lamp_projection * lamp_view  * modelsMatrix[modelParent];
            break;
        }
    }
    // mat4 mvp = (flags && 1 ? lamp_projection * lamp_view : projection * view) * modelsMatrix[modelParent];
    gl_Position = (mvp * vec4(position, 1));

    modelParentGeom = modelParent;
}
