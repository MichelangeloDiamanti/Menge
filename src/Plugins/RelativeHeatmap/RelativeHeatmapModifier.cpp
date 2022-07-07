#include "RelativeHeatmapModifier.h"

using namespace cimg_library;

namespace RelativeHeatmap {

using Menge::Agents::BaseAgent;
using Menge::Agents::PrefVelocity;
using Menge::BFSM::VelModFactory;
using Menge::BFSM::VelModifier;
using Menge::Math::Vector2;

using Menge::logger;
using Menge::Logger;

using Menge::Resource;
using Menge::ResourceException;

/////////////////////////////////////////////////////////////////////
//                   Implementation of RelativeHeatmapModifier
/////////////////////////////////////////////////////////////////////

RelativeHeatmapModifier::RelativeHeatmapModifier() {}

/////////////////////////////////////////////////////////////////////

RelativeHeatmapModifier::~RelativeHeatmapModifier(){};

/////////////////////////////////////////////////////////////////////

VelModifier* RelativeHeatmapModifier::copy() const { return new RelativeHeatmapModifier(); };

/////////////////////////////////////////////////////////////////////

void RelativeHeatmapModifier::registerAgent(const BaseAgent* agent) {
  // Here I can take care of initialization. I need to compute the size of the heatmap in
  // world units using the provided scale, and set the base agent neighbour distance accordingly

  int HMWidth = _relativeHeatmap->getWidth();
  int HMHeight = _relativeHeatmap->getHeight();
  float neighborDistance = HMWidth > HMHeight ? HMWidth : HMHeight;
  neighborDistance *= _scale;
  // agent->_neighborDist = neighborDistance;

  std::cout << "registering agent " << agent->_id
            << " with RelativeHeatmapModifier. The neighbor distance is: " << agent->_neighborDist
            << " computed neighborDistance: " << neighborDistance << std::endl;
}

/////////////////////////////////////////////////////////////////////

void RelativeHeatmapModifier::unregisterAgent(const BaseAgent* agent) {
  // std::cout << "unregistering agent " << agent->_id << " with RelativeHeatmapModifier" <<
  // std::endl;
}

/////////////////////////////////////////////////////////////////////

void RelativeHeatmapModifier::adaptPrefVelocity(const BaseAgent* agent, PrefVelocity& pVel) {
  int* rgb = _relativeHeatmap->getValueAt(200, 200);

  //std::cout << "R: " << rgb[0] << " G: " << rgb[1] << " B: " << rgb[2];
  std::cout << "There are " << agent->_nearAgents.size() << " neigbors" << std::endl;

  Vector2 dir = Vector2(0.0f, 0.0f);
  pVel.setSingle(dir);
}

void RelativeHeatmapModifier::setRelativeHeatmap(RelativeHeatmapPtr relativeHeatmap) {
  _relativeHeatmap = relativeHeatmap;
}

/////////////////////////////////////////////////////////////////////
//                   Implementation of RelativeHeatmapModifierFactory
/////////////////////////////////////////////////////////////////////

RelativeHeatmapModifierFactory::RelativeHeatmapModifierFactory() : VelModFactory() {
  // no properties yet
  _fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);

  _scaleID = _attrSet.addFloatAttribute("scale", false /*required*/, 1.f);
  _offsetXID = _attrSet.addFloatAttribute("offset_x", false /*required*/, 0.f);
  _offsetYID = _attrSet.addFloatAttribute("offset_y", false /*required*/, 0.f);
}

bool RelativeHeatmapModifierFactory::setFromXML(Menge::BFSM::VelModifier* modifier,
                                                TiXmlElement* node,
                                                const std::string& behaveFldr) const {
  RelativeHeatmapModifier* relativeHeatmapMod = dynamic_cast<RelativeHeatmapModifier*>(modifier);
  assert(relativeHeatmapMod != 0x0 &&
         "Trying to set property modifier properties on an incompatible object");

  if (!Menge::BFSM::VelModFactory::setFromXML(modifier, node, behaveFldr)) return false;

  // get the absolute path to the file name

  std::string fName;
  std::string path =
      Menge::os::path::join(2, behaveFldr.c_str(), _attrSet.getString(_fileNameID).c_str());
  Menge::os::path::absPath(path, fName);

  logger << Logger::INFO_MSG << "RelativeHeatmap file: " << fName;

  try {
    relativeHeatmapMod->setRelativeHeatmap(loadRelativeHeatmap(fName));
  } catch (ResourceException) {
    logger << Logger::ERR_MSG << "Couldn't instantiate the relative heatmap referenced on line ";
    logger << node->Row() << ".";
    return false;
  }

  relativeHeatmapMod->_scale = _attrSet.getFloat(_scaleID);
  relativeHeatmapMod->_offset =
      Menge::Vector2(_attrSet.getFloat(_offsetXID), _attrSet.getFloat(_offsetYID));

  std::cout << "scale: " << relativeHeatmapMod->_scale
            << " offset: " << relativeHeatmapMod->_offset;

  return true;
}

}  // namespace RelativeHeatmap