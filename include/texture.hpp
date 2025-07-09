#pragma once

class Texture{
    public:

  unsigned int renderID,Slot;
  const char *localPath;
  unsigned char *localBuffer;
  int width, height, BPP;
  Texture(const char* path);
  void Bind(unsigned int slot = 0);
  void Unbind() const;
  ~Texture();
};