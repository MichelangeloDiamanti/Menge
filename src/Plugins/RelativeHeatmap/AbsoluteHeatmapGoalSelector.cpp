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
  // return computeGoalBasedOnCircleAroundAgent(agent);
  return computeGoalBasedOnfieldView(agent);
}

Menge::BFSM::Goal* AbsoluteHeatmapGoalSelector::computeGoalBasedOnfieldView(
    const Menge::Agents::BaseAgent* agent) const {
  std::map<float, Vector2>
      sampledPointsScores;  // map containing "vision" points scored according to their color

  // Vector2 dir = agent->_orient;  // orientation of the lower body of the agent
  // Vector2 dir = agent->_velPref.getPreferred();  // orientation of the lower body of the agent
  Vector2 dir = agent->_orient;  // orientation of the lower body of the agent

  // sample and score a vision point in front of the agent
  Vector2 samplePoint = agent->_pos + dir * _lookMaxDist;
  int* rgb = new int[3]{0, 0, 0};
  int sampledPointScore =
      scoreRGBColor(samplePoint, _absoluteHeatmap->worldToMapColor(rgb, samplePoint), agent);

  // TODO: maybe we want to prefer the direction close to the agent's orientation
  // float angleScore = scoreAngle(agent->_orient, samplePoint);

  sampledPointsScores.insert({sampledPointScore, samplePoint});

  // sample and score vision points in the fieldview of the agent
  double degToRad = PI / 180;
  Vector2 sampleDir;

  for (float angle = -(_visionAngle / 2); angle < 0; angle = angle + _visionSampleAngleRate) {
    sampleDir = dir.rotate(angle * degToRad);
    float sampleDistIncrement = 0.1f;
    for (float sampleDist = 0.01f; sampleDist <= _visionRange; sampleDist += sampleDistIncrement) {
      samplePoint = agent->_pos + sampleDir * sampleDist;
      sampledPointScore =
          scoreRGBColor(samplePoint, _absoluteHeatmap->worldToMapColor(rgb, samplePoint), agent);
      sampledPointsScores.insert({sampledPointScore, samplePoint});
    }
  }
  for (float angle = _visionAngle / 2; angle > 0; angle = angle - _visionSampleAngleRate) {
    sampleDir = dir.rotate(angle * degToRad);
    float sampleDistIncrement = 0.1f;
    for (float sampleDist = 0.01f; sampleDist <= _visionRange; sampleDist += sampleDistIncrement) {
      samplePoint = agent->_pos + sampleDir * sampleDist;
      sampledPointScore =
          scoreRGBColor(samplePoint, _absoluteHeatmap->worldToMapColor(rgb, samplePoint), agent);
      sampledPointsScores.insert({sampledPointScore, samplePoint});
    }
  }

  auto firstPoint = sampledPointsScores.begin();

  // vision point with highest score (map highest is at the end)
  auto pointWithHighestScore = sampledPointsScores.end();
  pointWithHighestScore--;

  PointGoal* newGoal = new PointGoal(agent->_pos);

  // Vector2 goalPoint = _absoluteHeatmap->pixelToWorld(pointWithHighestScore->second);
  if (pointWithHighestScore->first > 0.f) {
    newGoal = new PointGoal(pointWithHighestScore->second);
    newGoal->setID(INT_MAX - _goals_count);
    _goals_count++;
  }

  delete rgb;

  return newGoal;
}

float AbsoluteHeatmapGoalSelector::scoreRGBColor(Vector2 point, int* color,
                                                 const BaseAgent* agent) const {
  float score = 0.f;
  score += (color[0] + color[1] + color[2]) / (255.0 * 3);
  if (score <= 0.f) return 0.f;
  
  score += angleToPoint(point, agent);
  score += distanceToPoint(point, agent);
  return score;
  // return color[0] * _redWeight + color[1] * _greenWeight + color[2] * _blueWeight;
  // return color[0] + color[1] + color[2];
  // return color[0];
}

float AbsoluteHeatmapGoalSelector::distanceToPoint(Vector2 point, const BaseAgent* agent) const {
  return clip((point - agent->_pos).Length(), 0, 1);
}

/** The angle to a point from the forward vector
 * of the agent */
float AbsoluteHeatmapGoalSelector::angleToPoint(Vector2 point, const BaseAgent* agent) const {
  Vector2 toPoint = point - agent->_pos;
  toPoint.normalize();
  Vector2 agentOrientationNormalized = agent->_orient;
  agentOrientationNormalized.normalize();

  float dotProduct = toPoint * agentOrientationNormalized;
  dotProduct = clip(dotProduct, -1.f, 1.f);

  return 1.f - (acosf(dotProduct) / (PI));
}

float AbsoluteHeatmapGoalSelector::clip(float n, float lower, float upper) const {
  return std::max(lower, std::min(n, upper));
}

Menge::BFSM::Goal* AbsoluteHeatmapGoalSelector::computeGoalBasedOnCircleAroundAgent(
    const Menge::Agents::BaseAgent* agent) const {
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
      if (_lookMaxDist > 0.f && _lookMinDist > 0.f) {
        if (dist_to_point < _lookMaxDist && dist_to_point > _lookMinDist) {
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
      } else {
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
  _lookMinDistID = _attrSet.addFloatAttribute("look_min", false /*required*/, -1.f);
  _lookMaxDistID = _attrSet.addFloatAttribute("look_max", false /*required*/, -1.f);
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
  gs->_lookMinDist = _attrSet.getFloat(_lookMinDistID);
  gs->_lookMaxDist = _attrSet.getFloat(_lookMaxDistID);

  return true;
}
}  // namespace RelativeHeatmap
