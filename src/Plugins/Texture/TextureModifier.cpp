#include "TextureModifier.h"

namespace Texture {

using Menge::Agents::BaseAgent;
using Menge::Agents::PrefVelocity;
using Menge::BFSM::VelModifier;
using Menge::Math::Vector2;

/////////////////////////////////////////////////////////////////////
//                   Implementation of FormationModifier
/////////////////////////////////////////////////////////////////////

TextureModifier::TextureModifier() {}

/////////////////////////////////////////////////////////////////////

TextureModifier::~TextureModifier(){};

/////////////////////////////////////////////////////////////////////

VelModifier* TextureModifier::copy() const { return new TextureModifier(); };

/////////////////////////////////////////////////////////////////////

void TextureModifier::adaptPrefVelocity(const BaseAgent* agent, PrefVelocity& pVel) {
  Vector2 dir = Vector2(0.0f, 0.0f);
  pVel.setSingle(dir);
}

/////////////////////////////////////////////////////////////////////
//                   Implementation of FormationModFactory
/////////////////////////////////////////////////////////////////////

TextureModifierFactory::TextureModifierFactory() : VelModFactory() {
  // no properties yet
  _fileNameID = _attrSet.addStringAttribute("file_name", true /*required*/);
}

bool TextureModifierFactory::setFromXML(Menge::BFSM::VelModifier* modifier, TiXmlElement* node,
                                        const std::string& behaveFldr) const {
  return true;
}

}  // namespace Texture