#pragma once 
#include <iostream>

void sendError(const char *str);
char *
readFile (const char *localPath, unsigned int *fileLenght);

struct vec3{
  float x,y,z;
  vec3(float x_, float y_, float z_){
    x = x_;
    y = y_;
    z = z_;
  }
  vec3(){}
  vec3(float arr[3]){
    x = arr[0];
    y = arr[1];
    z = arr[2];
  }
  vec3 operator +(vec3 other){
    return vec3(x+other.x, y + other.y, z + other.z);
  }
   vec3 operator -(vec3 other){
    return vec3(x-other.x, y - other.y, z - other.z);
  }
  float& operator[](int i){
    if(i <3 && i >=0)
      return ((float*)this)[i];
    sendError("vec3[x>2]");
    return x;
  }
};

struct vec2{
  float x,y;
  vec2(float x_, float y_){
    x = x_;
    y = y_;
  }
  vec2(){}
  vec2(float arr[2]){
    x = arr[0];
    y = arr[1];
  }
  vec2 operator +(vec2 other){
    return vec2(x+other.x, y + other.y);
  }
   vec2 operator -(vec2 other){
    return vec2(x-other.x, y - other.y);
  }
  float& operator[](int i){
    if(i <2 && i >=0)
      return ((float*)this)[i];
    sendError("vec2[x>1]");
    return x;
  }
};


std::ostream& operator<<(std::ostream& os, vec2 v);
std::ostream& operator<<(std::ostream& os, vec3 v);
