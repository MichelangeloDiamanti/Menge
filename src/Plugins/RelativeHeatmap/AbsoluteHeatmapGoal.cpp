#include "AbsoluteHeatmapGoal.h"

#include "MengeCore/BFSM/Goals/GoalPoint.h"

namespace RelativeHeatmap {

using Menge::logger;
using Menge::Logger;

using Menge::Resource;
using Menge::ResourceException;

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

/////////////////////////////////////////////////////////////////////
//                   Implementation of AbsoluteHeatmapGoal
/////////////////////////////////////////////////////////////////////

const std::string AbsoluteHeatmapGoal::NAME = "heatmap";

/////////////////////////////////////////////////////////////////////

AbsoluteHeatmapGoal::AbsoluteHeatmapGoal() : Goal() {}

AbsoluteHeatmapGoal::~AbsoluteHeatmapGoal() {}

/////////////////////////////////////////////////////////////////////

AbsoluteHeatmapGoal::AbsoluteHeatmapGoal(const Menge::Math::Vector2& p) : Goal() {
  _geometry = new Menge::Math::PointShape(p);
}

/////////////////////////////////////////////////////////////////////

AbsoluteHeatmapGoal::AbsoluteHeatmapGoal(float x, float y) : Goal() {
  _geometry = new Menge::Math::PointShape(Menge::Math::Vector2(x, y));
}

/////////////////////////////////////////////////////////////////////

void AbsoluteHeatmapGoal::setAbsoluteHeatmap(AbsoluteHeatmapPtr absoluteHeatmap) {
  _absoluteHeatmap = absoluteHeatmap;
}

Menge::Math::Vector2 AbsoluteHeatmapGoal::getGoalPosition() {
  //auto t1 = high_resolution_clock::now();

  int highestPixelValue = 0.0f;
  int x = 0;
  int y = 0;

  // simple for loop to get the brightest spot in the heatmap, and return the corresponding point in
  // the map TODO: we're checking for the [0] which is the Red channel. It's ok as long as it is
  // grayscale but should we adopt some other metric?
  for (int i = 0; i < _absoluteHeatmap->getWidth(); i++) {
    for (int j = 0; j < _absoluteHeatmap->getHeight(); j++) {
      if (highestPixelValue < _absoluteHeatmap->getValueAt(i, j)[0]) {
        highestPixelValue = _absoluteHeatmap->getValueAt(i, j)[0];
        x = i;
        y = j;
      }
    }
  }

  ////----------- execution time: ~64 ms ----------- 
  //auto t2 = high_resolution_clock::now();
  ///* Getting number of milliseconds as an integer. */
  //auto ms_int = duration_cast<milliseconds>(t2 - t1);

  //std::cout << "time for getGoalPosition: " << ms_int.count();

  return Menge::Math::Vector2(x, y);
}

/////////////////////////////////////////////////////////////////////
//                   Implementation of AbsoluteHeatmapGoalFactory
/////////////////////////////////////////////////////////////////////

AbsoluteHeatmapGoalFactory::AbsoluteHeatmapGoalFactory() {
  // no properties yet
  _fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);

  _scaleID = _attrSet.addFloatAttribute("scale", false /*required*/, 1.f);
}

bool AbsoluteHeatmapGoalFactory::setFromXML(Menge::BFSM::Goal* goal, TiXmlElement* node,
                                            const std::string& behaveFldr) const {
  AbsoluteHeatmapGoal* absHMGoal = dynamic_cast<AbsoluteHeatmapGoal*>(goal);
  assert(absHMGoal != 0x0 &&
         "Trying to set AbsoluteHeatmapGoal attributes on an incompatible object.");

  if (!GoalFactory::setFromXML(absHMGoal, node, behaveFldr)) return false;

  // get the absolute path to the file name

  std::string fName;
  std::string path =
      Menge::os::path::join(2, behaveFldr.c_str(), _attrSet.getString(_fileNameID).c_str());
  Menge::os::path::absPath(path, fName);

  logger << Logger::INFO_MSG << "AbsoluteHeatmap file: " << fName;

  // Try to instantiate a heatmap from the specified image file
  try {
    absHMGoal->setAbsoluteHeatmap(loadAbsoluteHeatmap(fName));
  } catch (ResourceException) {
    logger << Logger::ERR_MSG << "Couldn't instantiate the absolute heatmap referenced on line ";
    logger << node->Row() << ".";
    return false;
  }

  // Get the specified XML parameters and set it to the heatmap object
  absHMGoal->_absoluteHeatmap->_scale = _attrSet.getFloat(_scaleID);
  std::cout << "scale: " << absHMGoal->_absoluteHeatmap->_scale << std::endl;

  // read the heatmap and find the brightest spot.
  // the corresponding point is the goal
  Menge::Math::Vector2 goalPoint =
      absHMGoal->getAbsoluteHeatmap()->pixelToWorld(absHMGoal->getGoalPosition());
  Menge::Math::PointShape* geometry = new Menge::Math::PointShape(goalPoint);

  if (geometry != 0x0) {
    goal->setGeometry(geometry);
    return true;
  }

  return false;
}

}  // namespace RelativeHeatmap
