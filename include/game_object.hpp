#pragma once
#include "material.hpp"
#include "transform.hpp"

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
  void init();
};
