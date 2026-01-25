#include "draw_group.hpp"
#include <X11/X.h>
#include <cstdlib>
#include <unistd.h>
#include <stdlib.h>

#include "render_data.hpp"



unsigned int Render_Data::alloc_draw_group() {
    compacted_meshes.draw_groups = (DrawGroup*)realloc(compacted_meshes.draw_groups, sizeof(DrawGroup) * ++compacted_meshes.draw_groups_count);

    return compacted_meshes.draw_groups_count - 1;
}
