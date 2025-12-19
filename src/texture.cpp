#include "texture.hpp"
#include "glad/glad.h"


Texture::Texture(const char *path):localPath (path), width (0), height (0), BPP (0),position(0, 0)
{

}

// Texture::Texture (const char *path)
//     : localBuffer (nullptr), localPath (path), width (0), height (0), BPP (0),renderID(0)
// {
//         glGenTextures (1, &renderID);
//         glBindTexture (GL_TEXTURE_2D, renderID);
//         stbi_set_flip_vertically_on_load (1);
//         localBuffer = stbi_load (path, &width, &height, &BPP, 4);
//         glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//         glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//         glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//         glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//         glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
//                       GL_UNSIGNED_BYTE, localBuffer);
//         glGenerateMipmap(GL_TEXTURE_2D);
//         glBindTexture (GL_TEXTURE_2D, 0);

//         stbi_image_free (localBuffer);
// }
// Texture::~Texture ()
// {
//         glDeleteTextures (1, &renderID);
// }
// void
// Texture::Bind (unsigned int slot)
// {
//         Slot = slot;
//         glActiveTexture (GL_TEXTURE0 + Slot);
//         glBindTexture (GL_TEXTURE_2D, renderID);
// }
// void
// Texture::Unbind () const
// {
//         glBindTexture (GL_TEXTURE_2D, 0);
// }
