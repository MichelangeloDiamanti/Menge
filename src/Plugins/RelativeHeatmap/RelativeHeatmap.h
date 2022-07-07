#ifndef __RELATIVE_HEATMAP_H__
#define __RELATIVE_HEATMAP_H__

#include "MengeCore/resources/Resource.h"

#define cimg_use_png
#include "CImg.h"
#include "png.h"

using namespace cimg_library;

namespace RelativeHeatmap {

using Menge::Math::Vector2;

class RelativeHeatmap : public Menge::Resource {
 public:
  /*!
   @brief		Constructor

   @param	name		The name of the file containing the target relativeHeatmap.
   */
  RelativeHeatmap(const std::string& name);

  /*!
   @brief		Destructor
   */
  ~RelativeHeatmap();

  /*!
   @brief		Reports the message label for this resource.

   @returns	The label for relativeHeatmaps.
   */
  virtual const std::string& getLabel() const { return LABEL; }

  /*!
  @brief		Parses a relativeHeatmap definition and returns a pointer to it.
  This function works in conjunction with the ResourceManager. That is why it returns a
  pointer, not to a relativeHeatmap, but to a Resource. The ResourceManager uses it to load and
  instantiate Resource instances.
  @param		fileName		The path to the file containing the relativeHeatmap
  definition.
  @returns	A pointer to the new relativeHeatmap (if the file is valid), NULL if invalid.
  */
  static Menge::Resource* load(const std::string& fileName);

  Vector2 worldToPixel(Vector2 worldCoordinate);

  /*!
  @brief		reads the rgb value of the underlying relativeHeatmap at position x,y
  relative from the center of the heatmap

  @param		x x coordinate
  @param		y y coordinate
  @returns	A pointer to an RGB array.
  */
  int* getValueAt(int x, int y);

  ///*!
  //@brief		reads the rgb value of the underlying relativeHeatmap at position x,y relative
  //from the center of the heatmap

  //@param		x x coordinate
  //@param		y y coordinate
  //@returns	A pointer to an RGB array.
  //*/
  //int* getValueFromCenter(int x, int y);

  /*!
  @brief		provides the width of the heatmap
 
  @returns	The width of the heatmap.
  */
  int getWidth() { return _image.width(); };

  /*!
  @brief		provides the height of the heatmap
  @returns	The height of the heatmap.
  */
  int getHeight() { return _image.height(); };

  /*!
   @brief		The unique label for this data type to be used with resource management.
   */
  static const std::string LABEL;

  /*!
 @brief		The scale factor between heatmap pixels and world units.
                              Ex. if it is 0.1, it means that 1 pixel is 0.1 world units
 */
  float _scale;

  /*!
   @brief		The offset, from the center of the heatmap, where the agent is considered
   being.
   */
  Vector2 _offset;

  /*!
   @brief	the center of the heatmap in pixel units
  */
  Vector2 _center;

 protected:
  /*!
   @brief		Sets the relativeHeatmap.

   @param		image				    the relativeHeatmap to set.
   */
  void setRelativeHeatmap(CImg<unsigned char> image) { _image = image; };

  CImg<unsigned char> _image;
};

/*!
 @brief		The definition of the managed pointer for relativeHeatmap data
 */
typedef Menge::ResourcePtr<RelativeHeatmap> RelativeHeatmapPtr;

/*!
 @brief       load a relativeHeatmap
 @param	fileName	The name of the file containing the relativeHeatmap.
 @returns	The relativeHeatmapPtr containing the data.
 @throws		A ResourceException if the data is unable to be instantiated.
 */
RelativeHeatmapPtr loadRelativeHeatmap(const std::string& fileName) throw(Menge::ResourceException);

}  // namespace RelativeHeatmap

#endif