#include "game_object.hpp"
#include "render.hpp"
#include "common.hpp"


void GameObject::link_mesh(unsigned int index) {
    if(render.render_data.compacted_meshes.meshes_count <= index)
        logger.sendError("GameObject::link_mesh: index >= meshes_count");

    mesh_index = index;
}


void GameObject::init() {

}
