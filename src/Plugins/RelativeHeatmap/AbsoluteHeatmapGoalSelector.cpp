#include "AbsoluteHeatmapGoalSelector.h"

#include <cassert>

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/Goals/GoalPoint.h"

using namespace Menge;
using namespace Menge::BFSM;
using namespace Menge::Agents;

using namespace std;

namespace RelativeHeatmap {
/////////////////////////////////////////////////////////////////////
//                   Implementation of ExplicitGoalSelector
/////////////////////////////////////////////////////////////////////

AbsoluteHeatmapGoalSelector::AbsoluteHeatmapGoalSelector() : GoalSelector() {}

/////////////////////////////////////////////////////////////////////

Goal* AbsoluteHeatmapGoalSelector::getGoal(const BaseAgent* agent) const {
  cout << "ExplicitGoalSelector::getGoal()" << endl;

  // Vector2 goalPoint = getAbsoluteHeatmap()->pixelToWorld();

  int x = 0;
  int y = 0;
  int* rgb = new int[3]{0, 0, 0};
  float highestGoalScore = 0.0f;
  float goalScore = 0.0f;
  // simple for loop to get the brightest spot in the heatmap, and return the corresponding point
  // in the map TODO: we're checking for the [0] which is the Red channel. It's ok as long as it
  // is grayscale but should we adopt some other metric?
  for (int i = 0; i < _absoluteHeatmap->getWidth(); i++) {
    for (int j = 0; j < _absoluteHeatmap->getHeight(); j++) {
      float dist_to_point =
          _absoluteHeatmap->pixelToWorld(Menge::Math::Vector2(i, j)).distanceSq(agent->_pos);
      if (dist_to_point < 1000.f && dist_to_point > 300.0f) {
        _absoluteHeatmap->getValueAt(rgb, i, j);
        if (rgb[0] > 0) {
          goalScore = scoreGoal(i, j, agent, rgb);
          if (highestGoalScore < goalScore) {
            highestGoalScore = goalScore;
            x = i;
            y = j;
          }
        }
      }
    }
  }

  Vector2 goalPoint = _absoluteHeatmap->pixelToWorld(Menge::Math::Vector2(x, y));

  PointGoal* newGoal = new PointGoal(goalPoint);
  newGoal->setID(INT_MAX - _goals_count);
  _goals_count++;

  delete rgb;

  return newGoal;
}

/////////////////////////////////////////////////////////////////////

// score the goal based on the pixel value and the
// angle between the agent position and the goal point
// lower angles are preferred (agent walking straight lines)
// higher pixel values are preferred (agent walking towards the brightest spot)
float AbsoluteHeatmapGoalSelector::scoreGoal(int x, int y, const Menge::Agents::BaseAgent* agent,
                                             int* rgb) const {
  Vector2 goalPoint = _absoluteHeatmap->pixelToWorld(Menge::Math::Vector2(x, y));

  Vector2 agentPos = agent->_pos;
  Vector2 agentDir = agent->_orient;

  float angle = atan2(agentDir.y() - goalPoint.y(), agentDir.x() - goalPoint.x());
  angle = angle * 180 / PI;
  //  compute the score

  angle = fmod(angle, 360);
  if (angle < 0) angle += 360;

  float score = (rgb[0] / 255.f) - (angle / 360.f) / 2.f;

  if (score < 0.f) return 0.f;

  return score;
}

/////////////////////////////////////////////////////////////////////

void AbsoluteHeatmapGoalSelector::setGoalSet(std::map<size_t, GoalSet*>& goalSets) {
  if (goalSets.count(_goalSetID) == 1) {
    GoalSet* gs = goalSets[_goalSetID];
    size_t gid = _goalID;
    _goal = gs->getGoalByID(gid);
    if (_goal == 0x0) {
      std::stringstream ss;
      logger << Logger::ERR_MSG << "Goal Selector cannot find targeted goal (";
      logger << gid << ") in desired goal set (" << _goalSetID << ").";
      throw GoalSelectorException();
    }
  } else {
    logger << Logger::ERR_MSG;
    logger << "Explicit goal selector tried accessing a goal set that doesn't exist: ";
    logger << _goalSetID << "\n";
    throw GoalSelectorException();
  }
}

/////////////////////////////////////////////////////////////////////

void AbsoluteHeatmapGoalSelector::setAbsoluteHeatmap(AbsoluteHeatmapPtr absoluteHeatmap) {
  _absoluteHeatmap = absoluteHeatmap;
}

/////////////////////////////////////////////////////////////////////
//                   Implementation of ExplicitGoalSelectorFactory
/////////////////////////////////////////////////////////////////////

AbsoluteHeatmapGoalSelectorFactory::AbsoluteHeatmapGoalSelectorFactory() : GoalSelectorFactory() {
  _goalSetID = _attrSet.addSizeTAttribute("goal_set", true /*required*/);
  _goalID = _attrSet.addSizeTAttribute("goal", true /*required*/);

  // no properties yet
  _fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);

  _scaleID = _attrSet.addFloatAttribute("scale", false /*required*/, 1.f);
  _offsetXID = _attrSet.addFloatAttribute("offset_x", false /*required*/, 0.f);
  _offsetYID = _attrSet.addFloatAttribute("offset_y", false /*required*/, 0.f);
  _look_radiusID = _attrSet.addFloatAttribute("_look_radius", false /*required*/, -1.f);
}

/////////////////////////////////////////////////////////////////////

bool AbsoluteHeatmapGoalSelectorFactory::setFromXML(GoalSelector* selector, TiXmlElement* node,
                                                    const std::string& behaveFldr) const {
  AbsoluteHeatmapGoalSelector* gs = dynamic_cast<AbsoluteHeatmapGoalSelector*>(selector);
  assert(gs != 0x0 && "Trying to set explicit goal selector attributes on an incompatible object.");

  if (!GoalSelectorFactory::setFromXML(gs, node, behaveFldr)) return false;

  gs->setGoalSetID(_attrSet.getSizeT(_goalSetID));
  gs->setGoalID(_attrSet.getSizeT(_goalID));

  // get the absolute path to the file name
  std::string fName;
  std::string path =
      Menge::os::path::join(2, behaveFldr.c_str(), _attrSet.getString(_fileNameID).c_str());
  Menge::os::path::absPath(path, fName);

  logger << Logger::INFO_MSG << "AbsoluteHeatmap file: " << fName;

  // Try to instantiate a heatmap from the specified image file
  try {
    gs->setAbsoluteHeatmap(loadAbsoluteHeatmap(fName));
  } catch (ResourceException) {
    logger << Logger::ERR_MSG << "Couldn't instantiate the absolute heatmap referenced on line ";
    logger << node->Row() << ".";
    return false;
  }

  // Get the specified XML parameters and set it to the heatmap object

  // scale
  gs->_absoluteHeatmap->_scale = _attrSet.getFloat(_scaleID);
  std::cout << "scale: " << gs->_absoluteHeatmap->_scale << std::endl;

  // offset
  gs->_absoluteHeatmap->_offset =
      Menge::Vector2(_attrSet.getFloat(_offsetXID), _attrSet.getFloat(_offsetYID));

  // look radius
  gs->_look_radius = _attrSet.getFloat(_look_radiusID);

  return true;
}
}  // namespace RelativeHeatmap
