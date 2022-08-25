#include "RelativeHeatmapTransition.h"

namespace RelativeHeatmap {

using Menge::BFSM::Condition;
using Menge::BFSM::ConditionFactory;

using Menge::logger;
using Menge::Logger;

using Menge::Resource;
using Menge::ResourceException;

/////////////////////////////////////////////////////////////////////////
//                   Implementation of ColorCondition
/////////////////////////////////////////////////////////////////////////

ColorCondition::ColorCondition(const ColorCondition& cond)
    : Condition(cond), _relativeHeatmap(cond._relativeHeatmap) {}
 
////////////////////////////////////////////////////////////////////////////

ColorCondition::~ColorCondition() {}

////////////////////////////////////////////////////////////////////////////

bool ColorCondition::conditionMet(Menge::Agents::BaseAgent* agent, const Menge::BFSM::Goal* goal) {
  return false;
}

/////////////////////////////////////////////////////////////////////////

Menge::BFSM::Condition* ColorCondition::copy() { return new ColorCondition(*this); }

/////////////////////////////////////////////////////////////////////////

void ColorCondition::setRelativeHeatmap(RelativeHeatmapPtr relativeHeatmap) {
  _relativeHeatmap = relativeHeatmap;
}

/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//                   Implementation of ColorConditionFactory
/////////////////////////////////////////////////////////////////////////

ColorConditionFactory::ColorConditionFactory() : ConditionFactory() {
  // no properties yet
  _fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);

  _scaleID = _attrSet.addFloatAttribute("scale", false /*required*/, 1.f);
  _offsetXID = _attrSet.addFloatAttribute("offset_x", false /*required*/, 0.f);
  _offsetYID = _attrSet.addFloatAttribute("offset_y", false /*required*/, 0.f);
}

/////////////////////////////////////////////////////////////////////////

bool ColorConditionFactory::setFromXML(Menge::BFSM::Condition* condition, TiXmlElement* node,
                                       const std::string& behaveFldr) const {
  ColorCondition* colorCondition = dynamic_cast<ColorCondition*>(condition);
  assert(colorCondition != 0x0 &&
         "Trying to set the properties of a Color condition on an incompatible object");

  if (!ConditionFactory::setFromXML(colorCondition, node, behaveFldr)) {
    return false;
  }

  std::string fName;
  std::string path =
      Menge::os::path::join(2, behaveFldr.c_str(), _attrSet.getString(_fileNameID).c_str());
  Menge::os::path::absPath(path, fName);

  logger << Logger::INFO_MSG << "RelativeHeatmap file: " << fName;

  // Try to instantiate a heatmap from the specified image file
  try {
    colorCondition->setRelativeHeatmap(loadRelativeHeatmap(fName));
  } catch (ResourceException) {
    logger << Logger::ERR_MSG << "Couldn't instantiate the relative heatmap referenced on line ";
    logger << node->Row() << ".";
    return false;
  }

  // Get the specified XML parameters and set it to the heatmap object
  colorCondition->_relativeHeatmap->_scale = _attrSet.getFloat(_scaleID);
  colorCondition->_relativeHeatmap->_offset =
      Menge::Vector2(_attrSet.getFloat(_offsetXID), _attrSet.getFloat(_offsetYID));

  // Set the center of the heatmap (pixel units) based on the image size and offset
  float centerX = colorCondition->_relativeHeatmap->getWidth() / 2 +
                  colorCondition->_relativeHeatmap->_offset.x();
  float centerY = colorCondition->_relativeHeatmap->getHeight() / 2 +
                  colorCondition->_relativeHeatmap->_offset.y();
  colorCondition->_relativeHeatmap->_center = Vector2(centerX, centerY);

  std::cout << "scale: " << colorCondition->_relativeHeatmap->_scale
            << " offset: " << colorCondition->_relativeHeatmap->_offset
            << " center: " << colorCondition->_relativeHeatmap->_center << std::endl;

  return true;
}
}  // namespace RelativeHeatmap
