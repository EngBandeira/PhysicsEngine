#pragma once
#include "transform.hpp"

struct GameObjectGenData {
    char *name;
    char *uuid;
};

class GameObject {
  public:
  Transform tranform;
  char *name;
  GameObject();
};
