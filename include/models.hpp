#pragma once 

#include <vector>

#include "utils.hpp"

struct triangle{
  unsigned int t[3];//first sencond third
  triangle(unsigned int t_[3]){
    t[0] = t_[0];
    t[1] = t_[1];
    t[2] = t_[2];
  }
};
struct model
{
  std::vector<vec3> vertex;
  std::vector<triangle> triangles;
  model();
  model(const char* localPath);
  ~model();
};
