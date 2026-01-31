#version 460 core
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(rgba8, binding = 0) uniform image2D return_image;
uniform sampler2D texToShowFrom;

void main() {
    // if (gl_GlobalInvocationID.x > 1080 || gl_GlobalInvocationID.y > 1080)
    //     return;
    vec2 pt = gl_GlobalInvocationID.xy;
    pt.x = pt.x / 1920;
    pt.y = pt.y / 1088;

    imageStore(return_image, ivec2(gl_GlobalInvocationID.xy), texture(texToShowFrom, pt));
}
