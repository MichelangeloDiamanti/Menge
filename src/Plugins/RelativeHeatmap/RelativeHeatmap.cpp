#include "RelativeHeatmap.h"

using namespace cimg_library;

namespace RelativeHeatmap {

using Menge::logger;
using Menge::Logger;
using Menge::Resource;
using Menge::ResourceException;
using Menge::ResourceManager;

/////////////////////////////////////////////////////////////////////
//                   Implementation of RelativeHeatmap
/////////////////////////////////////////////////////////////////////

const std::string RelativeHeatmap::LABEL("relative_heatmap");

RelativeHeatmap::RelativeHeatmap(const std::string& name) : Resource(name) {}

RelativeHeatmap::~RelativeHeatmap() {}

Menge::Resource* RelativeHeatmap::load(const std::string& fileName) {
  RelativeHeatmap* relativeHeatmap = new RelativeHeatmap(fileName);

  relativeHeatmap->setRelativeHeatmap(CImg<unsigned char>(fileName.c_str()));

  return relativeHeatmap;
}

Vector2 RelativeHeatmap::worldToPixel(Vector2 worldCoordinate) {
  // apply the scale i.e. 0.01 -> 1 world unit = 100 pixels, so we divide
  Vector2 scaledWorldCoordinate = worldCoordinate / _scale;

  // compute the pixel coordinate, the y grows in the opposite direction
  Vector2 pixelCoordinate =
      Vector2(_center.x() + scaledWorldCoordinate.x(), _center.y() - scaledWorldCoordinate.y());

  return pixelCoordinate;
}

int* RelativeHeatmap::getValueAt(int x, int y) {
  int* rgb = new int[3]{0, 0, 0};

  if (x >= 0 && y >= 0 && x <= _image.width() && y <= _image.height()) {
    rgb[0] = (int)_image(x, y, 0, 0);
    rgb[1] = (int)_image(x, y, 0, 1);
    rgb[2] = (int)_image(x, y, 0, 2);
  }

  return rgb;
}

int* RelativeHeatmap::worldToMapColor(Vector2 worldCoordinate) {
  Vector2 pixelCoordinate = worldToPixel(worldCoordinate);
  return getValueAt(pixelCoordinate.x(), pixelCoordinate.y());
}

// int* RelativeHeatmap::getValueAt(int x, int y) {
//  int* rgb = new int[3];
//
//  rgb[0] = (int)_image(_center.x() + x, _center.y() + y, 0, 0);
//  rgb[1] = (int)_image(_center.x() + x, _center.y() + y, 0, 1);
//  rgb[2] = (int)_image(_center.x() + x, _center.y() + y, 0, 2);
//
//  return rgb;
//}

RelativeHeatmapPtr loadRelativeHeatmap(const std::string& fileName) throw(
    Menge::ResourceException) {
  Resource* rsrc =
      ResourceManager::getResource(fileName, &RelativeHeatmap::load, RelativeHeatmap::LABEL);

  if (rsrc == 0x0) {
    logger << Logger::ERR_MSG << "No resource available.";
    throw ResourceException();
  }
  RelativeHeatmap* relativeHeatmap = dynamic_cast<RelativeHeatmap*>(rsrc);
  if (relativeHeatmap == 0x0) {
    logger << Logger::ERR_MSG << "Resource with name " << fileName << " is not a relativeHeatmap.";
    throw ResourceException();
  }

  return RelativeHeatmapPtr(relativeHeatmap);
}

}  // namespace RelativeHeatmap
