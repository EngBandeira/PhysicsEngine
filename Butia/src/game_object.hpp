#pragma once
#include "material/material.hpp"
#include "transform/transform.hpp"

struct GameObjectGenData {
    char *name;
    char *uuid;
    unsigned int mesh_index = -1;
};

class GameObject {
  public:
  Transform tranform;
  char *name;
  unsigned int mesh_index = -1, draw_group = -1;
  void link_mesh(unsigned int index);
  GameObject() { }
};
