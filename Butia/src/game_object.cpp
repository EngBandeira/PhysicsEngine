#include "game_object.hpp"
#include "utils/common.hpp"
#include "butia.hpp"

void GameObject::link_mesh(unsigned int index) {
    if(butia->render_data.compacted_meshes.meshes_count <= index)
        logger.sendError("GameObject::link_mesh: index >= meshes_count");

    mesh_index = index;
}
