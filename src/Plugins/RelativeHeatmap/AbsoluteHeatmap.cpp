#include "AbsoluteHeatmap.h"

using namespace cimg_library;

namespace RelativeHeatmap {

using Menge::logger;
using Menge::Logger;
using Menge::Resource;
using Menge::ResourceException;
using Menge::ResourceManager;

/////////////////////////////////////////////////////////////////////
//                   Implementation of AbsoluteHeatmap
/////////////////////////////////////////////////////////////////////

const std::string AbsoluteHeatmap::LABEL("absolute_heatmap");

AbsoluteHeatmap::AbsoluteHeatmap(const std::string& name) : Resource(name) {}

AbsoluteHeatmap::~AbsoluteHeatmap() {}

Menge::Resource* AbsoluteHeatmap::load(const std::string& fileName) {
  AbsoluteHeatmap* AbsoluteHeatmap = new RelativeHeatmap::AbsoluteHeatmap(fileName);

  AbsoluteHeatmap->setAbsoluteHeatmap(CImg<unsigned char>(fileName.c_str()));

  return AbsoluteHeatmap;
}

Vector2 AbsoluteHeatmap::worldToPixel(Vector2 worldCoordinate) {
  // apply the scale i.e. 0.01 -> 1 world unit = 100 pixels, so we divide
  Vector2 scaledWorldCoordinate = worldCoordinate / _scale;

  // compute the pixel coordinate, the y grows in the opposite direction
  Vector2 pixelCoordinate = Vector2(scaledWorldCoordinate.x() + _offset.x(),
                                    _image.height() - (scaledWorldCoordinate.y() + _offset.y()));

  return pixelCoordinate;
}

// TODO: check this!! not sure if the scaling is correct, but otherwise it would be the same as worldToPixel?
Vector2 AbsoluteHeatmap::pixelToWorld(Vector2 pixelCoordinate) {
  // apply the scale i.e. 0.01 -> 1 world unit = 100 pixels, so we multiply
  Vector2 scaledPixelCoordinate = pixelCoordinate * _scale;

  // compute the world coordinate, the y grows in the opposite direction
  Vector2 worldCoordinate = Vector2(scaledPixelCoordinate.x() - _offset.x(),
                                    _image.height() - (scaledPixelCoordinate.y() - _offset.y()));

  return worldCoordinate;
}

void AbsoluteHeatmap::getValueAt(int* rgb, int x, int y) {
  //int* rgb = new int[3]{0, 0, 0};

  if (x >= 0 && y >= 0 && x <= _image.width() && y <= _image.height()) {
    rgb[0] = (int)_image(x, y, 0, 0);
    rgb[1] = (int)_image(x, y, 0, 1);
    rgb[2] = (int)_image(x, y, 0, 2);
  }

  //return rgb;
}

int* AbsoluteHeatmap::worldToMapColor(int* rgb, Vector2 worldCoordinate) {
  Vector2 pixelCoordinate = worldToPixel(worldCoordinate);
  getValueAt(rgb, pixelCoordinate.x(), pixelCoordinate.y());
  return rgb;
}

AbsoluteHeatmapPtr loadAbsoluteHeatmap(const std::string& fileName) throw(
    Menge::ResourceException) {
  Resource* rsrc =
      ResourceManager::getResource(fileName, &AbsoluteHeatmap::load, AbsoluteHeatmap::LABEL);

  if (rsrc == 0x0) {
    logger << Logger::ERR_MSG << "No resource available.";
    throw ResourceException();
  }
  AbsoluteHeatmap* AbsoluteHeatmap = dynamic_cast<RelativeHeatmap::AbsoluteHeatmap*>(rsrc);
  if (AbsoluteHeatmap == 0x0) {
    logger << Logger::ERR_MSG << "Resource with name " << fileName << " is not a AbsoluteHeatmap.";
    throw ResourceException();
  }

  return AbsoluteHeatmapPtr(AbsoluteHeatmap);
}

}  // namespace RelativeHeatmap
