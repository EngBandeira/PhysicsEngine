#include "texture.hpp"
#include "stb_image/stb_image.h"
#include <GL/gl.h>
Texture::Texture(const char *path): localBuffer(nullptr), localPath(path), width(0),height(0),BPP(0)
{
  glGenTextures(1,&renderID);
  glBindTexture(GL_TEXTURE_2D,renderID);
  stbi_set_flip_vertically_on_load(1);
  localBuffer = stbi_load(path,&width,&height,&BPP,4);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,width,height,
  0,GL_RGBA,GL_UNSIGNED_INT, localBuffer);

  glBindTexture(GL_TEXTURE_2D,0);


}
Texture::~Texture(){
  stbi_image_free(localBuffer);
  glDeleteTextures(1,&renderID);
}
void Texture::Bind(unsigned int slot){
  Slot = slot;
glActiveTexture(GL_TEXTURE0+Slot);
glBindTexture(GL_TEXTURE_2D, renderID);
}
void Texture::Unbind() const {
  glBindTexture(GL_TEXTURE_2D,0);
}