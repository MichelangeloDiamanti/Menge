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
  if (i >= _positions.size()) {
    throw AgentGeneratorFatalException(
        "ExternalPersistentAgentGenerator trying to access an agent "
        "outside of the specified population");
  }
  agt->_pos = addNoise(_positions[i]);
  _positionsSet++;
  _count++;
}

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ExternalPersistentAgentGenerator::addPosition(const Vector2& p) {
  _positions.push_back(p);
}

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ExternalPersistentAgentGenerator::removePosition(const Vector2& p) {
  _positions.erase(std::remove(_positions.begin(), _positions.end(), p), _positions.end());
}

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ExternalPersistentAgentGenerator::clearPositions() { _positions.clear(); }

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ExternalPersistentAgentGenerator::spawn() { _positionsSet = 0; }

//////////////////////////////////////////////////////////////////////////////

bool Menge::Agents::ExternalPersistentAgentGenerator::shouldGenerate(float time_step) {
  return (_positionsSet < _positions.size());
}

//////////////////////////////////////////////////////////////////////////////

size_t Menge::Agents::ExternalPersistentAgentGenerator::getSpawnRate() const {
  return _positions.size();
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
