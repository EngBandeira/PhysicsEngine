#include "draw_group.hpp"
#include <X11/X.h>
#include <cstdlib>
#include <unistd.h>
#include <stdlib.h>

#include "render_data.hpp"



unsigned int Render_Data::alloc_draw_group(const char *vertex_shaders, const char *geometry_shaders, const char *fragment_shaders) {
    compacted_meshes.draw_groups = (DrawGroup*)realloc(compacted_meshes.draw_groups, sizeof(DrawGroup) * ++compacted_meshes.draw_groups_count);
    compacted_meshes.draw_groups[compacted_meshes.draw_groups_count - 1] = DrawGroup();
    compacted_meshes.draw_groups[compacted_meshes.draw_groups_count - 1].init(vertex_shaders, geometry_shaders, fragment_shaders);
    return compacted_meshes.draw_groups_count - 1;
}
