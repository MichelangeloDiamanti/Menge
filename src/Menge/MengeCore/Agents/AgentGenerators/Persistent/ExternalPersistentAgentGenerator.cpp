#include "ExternalPersistentAgentGenerator.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Runtime/Logger.h"
#include "thirdParty/tinyxml.h"

//////////////////////////////////////////////////////////////////////////////
// 		Implementation of ExternalPersistentAgentGenerator
//////////////////////////////////////////////////////////////////////////////

size_t Menge::Agents::ExternalPersistentAgentGenerator::agentCount() { return _count; }

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ExternalPersistentAgentGenerator::setAgentPosition(size_t i, BaseAgent* agt) {
  if (i >= _posOrients.size()) {
    throw AgentGeneratorFatalException(
        "ExternalPersistentAgentGenerator trying to access an agent "
        "outside of the specified population");
  }
  agt->_pos = addNoise(_posOrients[i].position);
  _positionsSet++;
  _count++;
}

void Menge::Agents::ExternalPersistentAgentGenerator::setAgentOrientation(size_t i,
                                                                          BaseAgent* agt) {
  if (i >= _posOrients.size()) {
    throw AgentGeneratorFatalException(
        "ExternalPersistentAgentGenerator trying to access an agent "
        "outside of the specified population");
  }
  agt->_orient = addNoise(_posOrients[i].orientation);
}

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ExternalPersistentAgentGenerator::addPositionOrientation(const Vector2& p,
                                                                             const Vector2& o) {
  PositionOrientation po;
  po.position = p;
  po.orientation = o;
  _posOrients.push_back(po);
}

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ExternalPersistentAgentGenerator::removePositionOrientation(const Vector2& p) {
  for (size_t i = 0; i < _posOrients.size(); i++) {
    if (_posOrients[i].position == p) _posOrients.erase(_posOrients.begin() + i);
    return;
  }
}

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ExternalPersistentAgentGenerator::clearPositionOrientations() {
  _posOrients.clear();
}

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ExternalPersistentAgentGenerator::spawn() { _positionsSet = 0; }

//////////////////////////////////////////////////////////////////////////////

bool Menge::Agents::ExternalPersistentAgentGenerator::shouldGenerate(float time_step) {
  return (_positionsSet < _posOrients.size());
}

//////////////////////////////////////////////////////////////////////////////

size_t Menge::Agents::ExternalPersistentAgentGenerator::getSpawnRate() const {
  return _posOrients.size();
}

//////////////////////////////////////////////////////////////////////////////
// 		Implementation of ExternalPersistentAgentGeneratorFactory
//////////////////////////////////////////////////////////////////////////////

bool Menge::Agents::ExternalPersistentAgentGeneratorFactory::setFromXML(
    PersistentAgentGenerator* gen, TiXmlElement* node, const std::string& behaveFldr) const {
  ExternalPersistentAgentGenerator* cpGen = dynamic_cast<ExternalPersistentAgentGenerator*>(gen);
  assert(cpGen != 0x0 &&
         "Trying to set attributes of an external persistent agent generator component on an "
         "incompatible object");

  if (!PersistentAgentGeneratorFactory::setFromXML(cpGen, node, behaveFldr)) return false;

  return true;
}
