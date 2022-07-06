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

int* RelativeHeatmap::getValueAt(int x, int y) {
  int* rgb = new int[3];

  rgb[0] = (int)_image(x, y, 0, 0);
  rgb[1] = (int)_image(x, y, 0, 1);
  rgb[2] = (int)_image(x, y, 0, 2);

  return rgb;
}

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
