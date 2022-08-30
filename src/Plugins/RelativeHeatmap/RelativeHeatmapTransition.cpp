/*

License

Menge
Copyright © and trademark ™ 2012-14 University of North Carolina at Chapel Hill.
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation
for educational, research, and non-profit purposes, without fee, and without a
written agreement is hereby granted, provided that the above copyright notice,
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North
Carolina at Chapel Hill. The software program and documentation are supplied "as is,"
without any accompanying services from the University of North Carolina at Chapel
Hill or the authors. The University of North Carolina at Chapel Hill and the
authors do not warrant that the operation of the program will be uninterrupted
or error-free. The end-user understands that the program was developed for research
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

/*!
 *	@file		AircraftTransition.h
 *	@brief		Definition of transition condition used in aircraft
 *				loading and unloading.
 */

#include "RelativeHeatmapTransition.h"

#include "RelativeHeatmapConfig.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/BFSM/FSMEnumeration.h"
#include "MengeCore/BFSM/Transitions/ConditionFactory.h"
#include "MengeCore/Core.h"

#include <map>

namespace RelativeHeatmap {

using Menge::Logger;
using Menge::logger;
using Menge::Agents::BaseAgent;
using Menge::BFSM::Condition;
using Menge::BFSM::ConditionFactory;
using Menge::BFSM::Goal;
using Menge::Math::Vector2;

using Menge::Resource;
using Menge::ResourceException;

/////////////////////////////////////////////////////////////////////////
//                   Implementation of ColorCondition
/////////////////////////////////////////////////////////////////////////

/*!
   @brief		Constructor
*/

ColorCondition::ColorCondition()
    : Menge::BFSM::Condition(), _relativeHeatmap(), _conditionColorRGB() {
  _conditionColorRGB = new int[3];
}

/*!
   @brief		Copy Constructor
*/

ColorCondition::ColorCondition(const ColorCondition& cond)
    : Condition(cond),
      _relativeHeatmap(cond._relativeHeatmap),
      _conditionColorRGB(cond._conditionColorRGB) {}

/////////////////////////////////////////////////////////////////////////

ColorCondition::~ColorCondition() { delete[] _conditionColorRGB; }

/////////////////////////////////////////////////////////////////////////

bool ColorCondition::conditionMet(BaseAgent* agent, const Goal* goal) {
  // TODO: OPTIMIZE THIS
  const size_t NUM_AGENT = Menge::SIMULATOR->getNumAgents();
  for (size_t i = 0; i < NUM_AGENT; ++i) {
    const BaseAgent* testAgent = Menge::SIMULATOR->getAgent(i);
    // if this agent is in my box
    if (testAgent->_id != agent->_id) {
      Vector2 relativePosition = agent->_pos - testAgent->_pos;
      int* mapColor = _relativeHeatmap->worldToMapColor(relativePosition);
      //std::cout << "[" << mapColor[0] << ", " << mapColor[1] << ", " << mapColor[2] << "]"
      //          << std::endl;
      // if map color is the same as the one specified in the colorConditionRGB
      if (mapColor[0] == _conditionColorRGB[0] && mapColor[1] == _conditionColorRGB[1] &&
          mapColor[2] == _conditionColorRGB[2]) {
        //std::cout << "conditionMet" << std::endl;
        return true;
      }
    }
  }

  return false;
};

/////////////////////////////////////////////////////////////////////////

Condition* ColorCondition::copy() { return new ColorCondition(*this); }

/////////////////////////////////////////////////////////////////////////

void ColorCondition::setRelativeHeatmap(RelativeHeatmapPtr relativeHeatmap) {
  _relativeHeatmap = relativeHeatmap;
}

///////////////////////////////////////////////////////////////////////////
//                   Implementation of ColorConditionFactory
///////////////////////////////////////////////////////////////////////////

ColorConditionFactory::ColorConditionFactory() : ConditionFactory() {
  // no properties yet
  _fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);
  _colorRGB = _attrSet.addStringAttribute("color_rgb", true /*required*/);

  _scaleID = _attrSet.addFloatAttribute("scale", false /*required*/, 1.f);
  _offsetXID = _attrSet.addFloatAttribute("offset_x", false /*required*/, 0.f);
  _offsetYID = _attrSet.addFloatAttribute("offset_y", false /*required*/, 0.f);
}

/////////////////////////////////////////////////////////////////////////

bool ColorConditionFactory::setFromXML(Condition* condition, TiXmlElement* node,
                                       const std::string& behaveFldr) const {
  ColorCondition* cond = dynamic_cast<ColorCondition*>(condition);
  assert(cond != 0x0 &&
         "Trying to set the properties of a Color condition on an incompatible object");

  if (!ConditionFactory::setFromXML(cond, node, behaveFldr)) {
    return false;
  }

  std::string fName;
  std::string path =
      Menge::os::path::join(2, behaveFldr.c_str(), _attrSet.getString(_fileNameID).c_str());
  Menge::os::path::absPath(path, fName);

  logger << Logger::INFO_MSG << "RelativeHeatmap file: " << fName;

  // Try to instantiate a heatmap from the specified image file
  try {
    cond->setRelativeHeatmap(loadRelativeHeatmap(fName));
  } catch (ResourceException) {
    logger << Logger::ERR_MSG << "Couldn't instantiate the relative heatmap referenced on line ";
    logger << node->Row() << ".";
    return false;
  }

  // Get the specified XML parameters and set it to the heatmap object

  // Set the RGB color which will trigger the transition. It should be specified in the
  // behavior file as colorRGB = "R G B" with spaces as delimiter and R G B integers
  std::string colorRGB = _attrSet.getString(_colorRGB);
  std::string delimiter = " ";

  size_t pos = 0;
  std::vector<std::string> tokens;
  while ((pos = colorRGB.find(delimiter)) != std::string::npos) {
    tokens.insert(tokens.end(), colorRGB.substr(0, pos));
    colorRGB.erase(0, pos + delimiter.length());
  }

  tokens.insert(tokens.end(), colorRGB);

  if (tokens.size() != 3) {
    logger << Logger::ERR_MSG << "Couldn't parse the RGB color specified on line ";
    logger << node->Row() << ".";
    return false;
  }

  cond->_conditionColorRGB[0] = std::stoi(tokens[0]);
  cond->_conditionColorRGB[1] = std::stoi(tokens[1]);
  cond->_conditionColorRGB[2] = std::stoi(tokens[2]);

  // set scale and offset for the heatmap
  cond->_relativeHeatmap->_scale = _attrSet.getFloat(_scaleID);
  cond->_relativeHeatmap->_offset =
      Menge::Vector2(_attrSet.getFloat(_offsetXID), _attrSet.getFloat(_offsetYID));

  // Set the center of the heatmap (pixel units) based on the image size and offset
  float centerX = cond->_relativeHeatmap->getWidth() / 2 + cond->_relativeHeatmap->_offset.x();
  float centerY = cond->_relativeHeatmap->getHeight() / 2 + cond->_relativeHeatmap->_offset.y();
  cond->_relativeHeatmap->_center = Vector2(centerX, centerY);

  std::cout << "conditionColorRGB: [" << cond->_conditionColorRGB[0] << ", "
            << cond->_conditionColorRGB[1] << ", " << cond->_conditionColorRGB[0] << "]"
            << " scale: " << cond->_relativeHeatmap->_scale
            << " offset: " << cond->_relativeHeatmap->_offset
            << " center: " << cond->_relativeHeatmap->_center << std::endl;

  return true;
}
}  // namespace RelativeHeatmap
