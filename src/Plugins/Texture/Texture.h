#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "MengeCore/resources/Resource.h"

#define cimg_use_png
#include "CImg.h"
#include "png.h"

namespace Textures {

using namespace cimg_library;

class Texture : public Menge::Resource {
 public:
  /*!
   @brief		Constructor

   @param	name		The name of the file containing the target texture.
   */
  Texture(const std::string& name);

  /*!
   @brief		Destructor
   */
  ~Texture();

  /*!
   @brief		Reports the message label for this resource.

   @returns	The label for textures.
   */
  virtual const std::string& getLabel() const { return LABEL; }

  /*!
  @brief		Parses a texture definition and returns a pointer to it.
  This function works in conjunction with the ResourceManager. That is why it returns a
  pointer, not to a texture, but to a Resource. The ResourceManager uses it to load and
  instantiate Resource instances.
 
  @param		fileName		The path to the file containing the texture
  definition.
  @returns	A pointer to the new texture (if the file is valid), NULL if invalid.
  */
  static Menge::Resource* load(const std::string& fileName);

  /*!
  @brief		reads the rgb value of the underlying texture at position x,y
 
  @param		x x coordinate
  @param		y y coordinate
  @returns	A pointer to an RGB array.
  */
  int* getValueAt(int x, int y);


  /*!
   @brief		The unique label for this data type to be used with resource management.
   */
  static const std::string LABEL;

 protected:
  /*!
   @brief		Sets the texture.

   @param		image				    the texture to set.
   */
  void setTexture(CImg<unsigned char> image) { _image = image; };

  CImg<unsigned char> _image;
};

/*!
 @brief		The definition of the managed pointer for texture data
 */
typedef Menge::ResourcePtr<Texture> TexturePtr;

/*!
 @brief       load a texture
 @param	fileName	The name of the file containing the texture.
 @returns	The texturePtr containing the data.
 @throws		A ResourceException if the data is unable to be instantiated.
 */
TexturePtr loadTexture(const std::string& fileName) throw(Menge::ResourceException);

}  // namespace Textures

#endif