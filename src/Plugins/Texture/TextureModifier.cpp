#include "TextureModifier.h"

// using namespace cimg_library;

namespace Textures {

using Menge::Agents::BaseAgent;
using Menge::Agents::PrefVelocity;
using Menge::BFSM::VelModFactory;
using Menge::BFSM::VelModifier;
using Menge::Math::Vector2;

using Menge::logger;
using Menge::Logger;

/////////////////////////////////////////////////////////////////////
//                   Implementation of FormationModifier
/////////////////////////////////////////////////////////////////////

TextureModifier::TextureModifier() {}

/////////////////////////////////////////////////////////////////////

TextureModifier::~TextureModifier(){};

/////////////////////////////////////////////////////////////////////

VelModifier* TextureModifier::copy() const { return new TextureModifier(); };

/////////////////////////////////////////////////////////////////////

void TextureModifier::adaptPrefVelocity(const BaseAgent* agent, PrefVelocity& pVel) {
  Vector2 dir = Vector2(0.0f, 0.0f);
  pVel.setSingle(dir);
}

/////////////////////////////////////////////////////////////////////
//                   Implementation of FormationModFactory
/////////////////////////////////////////////////////////////////////

TextureModifierFactory::TextureModifierFactory() : VelModFactory() {
  // no properties yet
  _fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);
}

bool TextureModifierFactory::setFromXML(Menge::BFSM::VelModifier* modifier, TiXmlElement* node,
                                        const std::string& behaveFldr) const {
  TextureModifier* textureMod = dynamic_cast<TextureModifier*>(modifier);
  assert(textureMod != 0x0 &&
         "Trying to set property modifier properties on an incompatible object");

  if (!Menge::BFSM::VelModFactory::setFromXML(modifier, node, behaveFldr)) return false;

  // get the absolute path to the file name

  std::string fName;
  std::string path =
      Menge::os::path::join(2, behaveFldr.c_str(), _attrSet.getString(_fileNameID).c_str());
  Menge::os::path::absPath(path, fName);

  logger << Logger::INFO_MSG << "Texture path: " << path;

  // CImg<unsigned char> image(path.c_str());

  // logger << Logger::INFO_MSG << "Texture width: " << image.width() << " height: " <<
  // image.height();

  FILE* fp = fopen(path.c_str(), "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);
  png_byte color_type = png_get_color_type(png, info);

  logger << Logger::INFO_MSG << "Texture width: " << width << " height: " << height;

  return true;
}

}  // namespace Textures