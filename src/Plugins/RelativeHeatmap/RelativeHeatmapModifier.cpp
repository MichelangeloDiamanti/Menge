#include "RelativeHeatmapModifier.h"

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
  neighborDistance *= _relativeHeatmap->_scale;
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
  //          << " with RelativeHeatmapModifier. The neighbor distance is: " << agent->_neighborDist
  //          << " computed neighborDistance: " << neighborDistance << std::endl;
}

/////////////////////////////////////////////////////////////////////

void RelativeHeatmapModifier::unregisterAgent(const BaseAgent* agent) {
  // std::cout << "unregistering agent " << agent->_id << " with RelativeHeatmapModifier" <<
  // std::endl;
}

/////////////////////////////////////////////////////////////////////

void RelativeHeatmapModifier::adaptPrefVelocity(const BaseAgent* agent, PrefVelocity& pVel) {
  // int* rgb = _relativeHeatmap->getValueAt(200, 200);
  // std::cout << "R: " << rgb[0] << " G: " << rgb[1] << " B: " << rgb[2];

  for (NearAgent neighbour : agent->_nearAgents) {
    Vector2 neighbourRelativePosition = neighbour.agent->_pos - agent->_pos;
    // neighbourDirection.normalize();

    Vector2 neighbourRelativePositionPixelUnits =
        _relativeHeatmap->worldToPixel(neighbourRelativePosition);

    int* rgb = _relativeHeatmap->getValueAt(neighbourRelativePositionPixelUnits.x(),
                                            neighbourRelativePositionPixelUnits.y());

    // Kendon social zone "green"
    if (rgb[0] == 0 && rgb[1] == 255 && rgb[2] == 0) {
      std::cout << "Agent " << neighbour.agent->_id << " is in the social zone" << std::endl;
    }
    // Kendon personal zone "orange"
    else if (rgb[0] == 255 && rgb[1] == 128 && rgb[2] == 0) {
      std::cout << "Agent " << neighbour.agent->_id << " is in the personal zone" << std::endl;
    }
    // Kendon intimate zone "red"
    else if (rgb[0] == 255 && rgb[1] == 0 && rgb[2] == 0) {
      std::cout << "Agent " << neighbour.agent->_id << " is in the intimate zone" << std::endl;
    }

    // std::cout << "Agent " << neighbour.agent->_id << " is a neighbour at distance "
    //          << neighbour.distanceSquared
    //          << " relative position (world units): " << neighbourRelativePosition
    //          << " relative position (pixel units): " << neighbourRelativePositionPixelUnits
    //          << "R: " << rgb[0] << " G: " << rgb[1] << " B: " << rgb[2] << std::endl;
  }

  // Vector2 dir = Vector2(0.0f, 0.0f);
  // pVel.setSingle(dir);
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

  // Try to instantiate a heatmap from the specified image file
  try {
    relativeHeatmapMod->setRelativeHeatmap(loadRelativeHeatmap(fName));
  } catch (ResourceException) {
    logger << Logger::ERR_MSG << "Couldn't instantiate the relative heatmap referenced on line ";
    logger << node->Row() << ".";
    return false;
  }

  // Get the specified XML parameters and set it to the heatmap object
  relativeHeatmapMod->_relativeHeatmap->_scale = _attrSet.getFloat(_scaleID);
  relativeHeatmapMod->_relativeHeatmap->_offset =
      Menge::Vector2(_attrSet.getFloat(_offsetXID), _attrSet.getFloat(_offsetYID));

  // Set the center of the heatmap (pixel units) based on the image size and offset
  float centerX = relativeHeatmapMod->_relativeHeatmap->getWidth() / 2 +
                  relativeHeatmapMod->_relativeHeatmap->_offset.x();
  float centerY = relativeHeatmapMod->_relativeHeatmap->getHeight() / 2 +
                  relativeHeatmapMod->_relativeHeatmap->_offset.y();
  relativeHeatmapMod->_relativeHeatmap->_center = Vector2(centerX, centerY);

  std::cout << "scale: " << relativeHeatmapMod->_relativeHeatmap->_scale
            << " offset: " << relativeHeatmapMod->_relativeHeatmap->_offset
            << " center: " << relativeHeatmapMod->_relativeHeatmap->_center << std::endl;

  return true;
}

}  // namespace RelativeHeatmap