#pragma once
#include "material.hpp"
#include "transform.hpp"

struct GameObjectGenData {
    char *name;
    char *uuid;
};

class GameObject {
  public:
  Transform tranform;
  Material material;
  char *name;
  unsigned int mesh_index = -1;
  GameObject();
};
