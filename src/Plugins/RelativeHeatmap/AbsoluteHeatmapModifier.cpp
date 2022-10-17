#include "AbsoluteHeatmapModifier.h"

using namespace cimg_library;

namespace RelativeHeatmap {

using Menge::Agents::BaseAgent;
using Menge::Agents::NearAgent;
using Menge::Agents::PrefVelocity;
using Menge::BFSM::VelModFactory;
using Menge::BFSM::VelModifier;
using Menge::Math::Vector2;

using Menge::logger;
using Menge::Logger;

using Menge::Resource;
using Menge::ResourceException;

/////////////////////////////////////////////////////////////////////
//                   Implementation of AbsoluteHeatmapModifier
/////////////////////////////////////////////////////////////////////

AbsoluteHeatmapModifier::AbsoluteHeatmapModifier() {}

/////////////////////////////////////////////////////////////////////

AbsoluteHeatmapModifier::~AbsoluteHeatmapModifier(){};

/////////////////////////////////////////////////////////////////////

VelModifier* AbsoluteHeatmapModifier::copy() const { return new AbsoluteHeatmapModifier(); };

/////////////////////////////////////////////////////////////////////

void AbsoluteHeatmapModifier::registerAgent(const BaseAgent* agent) {
  // Here I can take care of initialization. I need to compute the size of the heatmap in
  // world units using the provided scale, and set the base agent neighbour distance accordingly

  int HMWidth = _absoluteHeatmap->getWidth();
  int HMHeight = _absoluteHeatmap->getHeight();
  float neighborDistance = HMWidth > HMHeight ? HMWidth : HMHeight;
  neighborDistance *= _absoluteHeatmap->_scale;
  // agent->_neighborDist = neighborDistance;

  // TODO: I cannot set the agent's _neighborDist because the agent is passed as a constant.
  // I probably could do this if I was implementing a new simulator and agent type, but then
  // this plugin would no longer be a simple velocity modifier i.e. can't be combined with other
  // pedestrian models. So, we might need to replicate here the "scan" for neighbors (if possible).
  // For now, we just log this waring to inform the user, who can then adjust the _neighborDist
  // parameter from the scene specification file.
  if (agent->_neighborDist < neighborDistance) {
    logger << Logger::WARN_MSG << "The neighbor distance for agent " << agent->_id
           << " is not sufficient to cover the whole heatmap.";
  }

  // std::cout << "registering agent " << agent->_id
  //          << " with AbsoluteHeatmapModifier. The neighbor distance is: " << agent->_neighborDist
  //          << " computed neighborDistance: " << neighborDistance << std::endl;
}

/////////////////////////////////////////////////////////////////////

void AbsoluteHeatmapModifier::unregisterAgent(const BaseAgent* agent) {
  // std::cout << "unregistering agent " << agent->_id << " with AbsoluteHeatmapModifier" <<
  // std::endl;
}

/////////////////////////////////////////////////////////////////////

void AbsoluteHeatmapModifier::adaptPrefVelocity(const BaseAgent* agent, PrefVelocity& pVel) {
  //const auto p1 = std::chrono::system_clock::now();

  //std::cout << std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count()
  //          << "_hasSubGoal: " << _hasSubGoal;

  if (!_hasSubGoal) {
    std::map<float, Vector2>
        sampledPointsScores;  // map containing "vision" points scored according to their color

    // Vector2 dir = agent->_orient;  // orientation of the lower body of the agent
    Vector2 dir = pVel.getPreferred();  // orientation of the lower body of the agent

    // sample and score a vision point in front of the agent
    Vector2 samplePoint = agent->_pos + dir * _visionRange;
    int sampledPointScore = scoreRGBColor(_absoluteHeatmap->worldToMapColor(samplePoint));

    // TODO: maybe we want to prefer the direction close to the agent's orientation
    // float angleScore = scoreAngle(agent->_orient, samplePoint);

    sampledPointsScores.insert({sampledPointScore, samplePoint});

    // sample and score vision points in the fieldview of the agent
    double degToRad = M_PI / 180;
    Vector2 sampleDir;

    for (float angle = -(_visionAngle / 2); angle < 0; angle = angle + _visionSampleAngleRate) {
      sampleDir = dir.rotate(angle * degToRad);
      samplePoint = agent->_pos + sampleDir * _visionRange;
      sampledPointScore = scoreRGBColor(_absoluteHeatmap->worldToMapColor(samplePoint));
      sampledPointsScores.insert({sampledPointScore, samplePoint});
    }
    for (float angle = _visionAngle / 2; angle > 0; angle = angle - _visionSampleAngleRate) {
      sampleDir = dir.rotate(angle * degToRad);
      samplePoint = agent->_pos + sampleDir * _visionRange;
      sampledPointScore = scoreRGBColor(_absoluteHeatmap->worldToMapColor(samplePoint));
      sampledPointsScores.insert({sampledPointScore, samplePoint});
    }

    auto firstPoint = sampledPointsScores.begin();

    // vision point with highest score (map highest is at the end)
    auto pointWithHighestScore = sampledPointsScores.end();
    pointWithHighestScore--;

    // std::cout << "agent " << agent->_id << " has highest score " << pointWithHighestScore->first
    //           << " at point " << pointWithHighestScore->second << std::endl;

    _heatmapSubGoal = pointWithHighestScore->second;
    _heatmapSubGoalScore = pointWithHighestScore->first;

    if (_heatmapSubGoalScore > 0.0) _hasSubGoal = true;
    //std::cout << "_heatmapSubGoalScore: " << _heatmapSubGoalScore << "firstPointScore" << firstPoint->first;
  } else {
    float distanceToSubGoal = _heatmapSubGoal.distanceSq(agent->_pos);

    if (distanceToSubGoal < _minDistanceToSubGoal) {
      _hasSubGoal = false;
    }
  }

  if (_hasSubGoal) {
    pVel.setSpeed(agent->_prefSpeed);
    Vector2 adjustedDir = _heatmapSubGoal - agent->_pos;
    adjustedDir.normalize();
    pVel.setSingle(adjustedDir);
  }
  //std::cout << std::endl;
}

int AbsoluteHeatmapModifier::scoreRGBColor(int* color) {
  return color[0] * _redWeight + color[1] * _greenWeight + color[2] * _blueWeight;
}

float AbsoluteHeatmapModifier::scoreAngle(Vector2 agentOrientation, Vector2 velocityOrientation) {
  float dot =
      agentOrientation._x * velocityOrientation._x +
      agentOrientation._y * velocityOrientation._y;  // dot product between[x1, y1] and [ x2, y2 ]
  float det = agentOrientation._x * velocityOrientation._y -
              agentOrientation._y * velocityOrientation._x;  // determinant
  float angle = atan2(det, dot);                             // atan2(y, x) or atan2(sin, cos)
  return angle;
}

void AbsoluteHeatmapModifier::setAbsoluteHeatmap(AbsoluteHeatmapPtr absoluteHeatmap) {
  _absoluteHeatmap = absoluteHeatmap;
}

/////////////////////////////////////////////////////////////////////
//                   Implementation of AbsoluteHeatmapModifierFactory
/////////////////////////////////////////////////////////////////////

AbsoluteHeatmapModifierFactory::AbsoluteHeatmapModifierFactory() : VelModFactory() {
  // no properties yet
  _fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);

  _scaleID = _attrSet.addFloatAttribute("scale", false /*required*/, 1.f);
}

bool AbsoluteHeatmapModifierFactory::setFromXML(Menge::BFSM::VelModifier* modifier,
                                                TiXmlElement* node,
                                                const std::string& behaveFldr) const {
  AbsoluteHeatmapModifier* absoluteHeatmapMod = dynamic_cast<AbsoluteHeatmapModifier*>(modifier);
  assert(absoluteHeatmapMod != 0x0 &&
         "Trying to set property modifier properties on an incompatible object");

  if (!Menge::BFSM::VelModFactory::setFromXML(modifier, node, behaveFldr)) return false;

  // get the absolute path to the file name

  std::string fName;
  std::string path =
      Menge::os::path::join(2, behaveFldr.c_str(), _attrSet.getString(_fileNameID).c_str());
  Menge::os::path::absPath(path, fName);

  logger << Logger::INFO_MSG << "AbsoluteHeatmap file: " << fName;

  // Try to instantiate a heatmap from the specified image file
  try {
    absoluteHeatmapMod->setAbsoluteHeatmap(loadAbsoluteHeatmap(fName));
  } catch (ResourceException) {
    logger << Logger::ERR_MSG << "Couldn't instantiate the relative heatmap referenced on line ";
    logger << node->Row() << ".";
    return false;
  }

  // Get the specified XML parameters and set it to the heatmap object
  absoluteHeatmapMod->_absoluteHeatmap->_scale = _attrSet.getFloat(_scaleID);

  std::cout << "scale: " << absoluteHeatmapMod->_absoluteHeatmap->_scale << std::endl;

  return true;
}

}  // namespace RelativeHeatmap