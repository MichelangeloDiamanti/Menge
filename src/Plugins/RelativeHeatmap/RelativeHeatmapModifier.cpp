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
//                   Implementation of FormationModifier
/////////////////////////////////////////////////////////////////////

RelativeHeatmapModifier::RelativeHeatmapModifier() {}

/////////////////////////////////////////////////////////////////////

RelativeHeatmapModifier::~RelativeHeatmapModifier(){};

/////////////////////////////////////////////////////////////////////

VelModifier* RelativeHeatmapModifier::copy() const { return new RelativeHeatmapModifier(); };

/////////////////////////////////////////////////////////////////////

void RelativeHeatmapModifier::adaptPrefVelocity(const BaseAgent* agent, PrefVelocity& pVel) {
  int* rgb = _relativeHeatmap->getValueAt(200, 200);

  std::cout << "R: " << rgb[0] << " G: " << rgb[1] << " B: " << rgb[2];

  Vector2 dir = Vector2(0.0f, 0.0f);
  pVel.setSingle(dir);
}

void RelativeHeatmapModifier::setRelativeHeatmap(RelativeHeatmapPtr relativeHeatmap) {
  _relativeHeatmap = relativeHeatmap;
}

/////////////////////////////////////////////////////////////////////
//                   Implementation of FormationModFactory
/////////////////////////////////////////////////////////////////////

RelativeHeatmapModifierFactory::RelativeHeatmapModifierFactory() : VelModFactory() {
  // no properties yet
  _fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);
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
    logger << Logger::ERR_MSG << "Couldn't instantiate the formation referenced on line ";
    logger << node->Row() << ".";
    return false;
  }

  return true;
}

}  // namespace RelativeHeatmap