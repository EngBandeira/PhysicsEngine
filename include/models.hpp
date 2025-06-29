#pragma once 

#include "utils.hpp"



struct model
{
  vec3 *vertex;
  model();
  model(const char* localPath);
  ~model();
};
