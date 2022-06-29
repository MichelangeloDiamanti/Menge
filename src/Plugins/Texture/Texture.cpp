#include "Texture.h"

using namespace cimg_library;

namespace Textures {

using Menge::logger;
using Menge::Logger;
using Menge::Resource;
using Menge::ResourceException;
using Menge::ResourceManager;

/////////////////////////////////////////////////////////////////////
//                   Implementation of Texture
/////////////////////////////////////////////////////////////////////

const std::string Texture::LABEL("texture");

Texture::Texture(const std::string& name) : Resource(name) {}

Texture::~Texture() {}

Menge::Resource* Texture::load(const std::string& fileName) {
  Texture* texture = new Texture(fileName);

  texture->setTexture(CImg<unsigned char>(fileName.c_str()));

  return texture;

}

int* Texture::getValueAt(int x, int y) {
  int* rgb = new int[3];

  rgb[0] = (int)_image(x, y, 0, 0);
  rgb[1] = (int)_image(x, y, 0, 1);
  rgb[2] = (int)_image(x, y, 0, 2);

  return rgb;
}

TexturePtr loadTexture(const std::string& fileName) throw(Menge::ResourceException) {
  Resource* rsrc = ResourceManager::getResource(fileName, &Texture::load, Texture::LABEL);

  if (rsrc == 0x0) {
    logger << Logger::ERR_MSG << "No resource available.";
    throw ResourceException();
  }
  Texture* texture = dynamic_cast<Texture*>(rsrc);
  if (texture == 0x0) {
    logger << Logger::ERR_MSG << "Resource with name " << fileName << " is not a texture.";
    throw ResourceException();
  }

  return TexturePtr(texture);
}

}  // namespace Textures
