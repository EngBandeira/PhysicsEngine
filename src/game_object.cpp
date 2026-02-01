#include "game_object.hpp"
#include "render.hpp"
#include "common.hpp"


void GameObject::link_mesh(unsigned int index) {
    if(render.render_data.compacted_meshes.meshes_count <= index)
        logger.sendError("GameObject::link_mesh: index >= meshes_count");

    mesh_index = index;
}


unsigned int GameObject::add_script(char* script) {
    scripts = (unsigned int*)realloc(scripts, sizeof(int) * ++scripts_count);
    scripts[scripts_count - 1] = render.scripts_manager.get_script(script);

    return scripts_count - 1;
}




void GameObject::init() {
    scripts = (unsigned int*)malloc(0);
}
