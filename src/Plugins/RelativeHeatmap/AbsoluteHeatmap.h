#ifndef __ABSOLUTE_HEATMAP_H__
#define __ABSOLUTE_HEATMAP_H__

#include "MengeCore/resources/Resource.h"

#define cimg_use_png
#include "CImg.h"
#include "png.h"

using namespace cimg_library;

namespace RelativeHeatmap {

using Menge::Math::Vector2;

class AbsoluteHeatmap : public Menge::Resource {
 public:
  /*!
   @brief		Constructor

   @param	name		The name of the file containing the target AbsoluteHeatmap.
   */
  AbsoluteHeatmap(const std::string& name);

  /*!
   @brief		Destructor
   */
  ~AbsoluteHeatmap();

  /*!
   @brief		Reports the message label for this resource.

   @returns	The label for AbsoluteHeatmaps.
   */
  virtual const std::string& getLabel() const { return LABEL; }

  /*!
  @brief		Parses a AbsoluteHeatmap definition and returns a pointer to it.
  This function works in conjunction with the ResourceManager. That is why it returns a
  pointer, not to a AbsoluteHeatmap, but to a Resource. The ResourceManager uses it to load and
  instantiate Resource instances.
  @param		fileName		The path to the file containing the AbsoluteHeatmap
  definition.
  @returns	A pointer to the new AbsoluteHeatmap (if the file is valid), NULL if invalid.
  */
  static Menge::Resource* load(const std::string& fileName);

  Vector2 worldToPixel(Vector2 worldCoordinate);

  /*!
  @brief		reads the rgb value of the underlying AbsoluteHeatmap at position x,y
  relative from the center of the heatmap

  @param		x x coordinate
  @param		y y coordinate
  @returns	A pointer to an RGB array.
  */
  int* getValueAt(int x, int y);

  int* worldToMapColor(Vector2 worldCoordinate);

  ///*!
  //@brief		reads the rgb value of the underlying AbsoluteHeatmap at position x,y relative
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

 protected:
  /*!
   @brief		Sets the AbsoluteHeatmap.

   @param		image				    the AbsoluteHeatmap to set.
   */
  void setAbsoluteHeatmap(CImg<unsigned char> image) { _image = image; };

  CImg<unsigned char> _image;
};

/*!
 @brief		The definition of the managed pointer for AbsoluteHeatmap data
 */
typedef Menge::ResourcePtr<AbsoluteHeatmap> AbsoluteHeatmapPtr;

/*!
 @brief       load a AbsoluteHeatmap
 @param	fileName	The name of the file containing the AbsoluteHeatmap.
 @returns	The AbsoluteHeatmapPtr containing the data.
 @throws		A ResourceException if the data is unable to be instantiated.
 */
AbsoluteHeatmapPtr loadAbsoluteHeatmap(const std::string& fileName) throw(Menge::ResourceException);

}  // namespace RelativeHeatmap

#endif