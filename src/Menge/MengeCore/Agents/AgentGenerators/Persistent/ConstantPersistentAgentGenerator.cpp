#include "ConstantPersistentAgentGenerator.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Runtime/Logger.h"
#include "thirdParty/tinyxml.h"

//////////////////////////////////////////////////////////////////////////////
// 		Implementation of ConstantPersistentAgentGenerator
//////////////////////////////////////////////////////////////////////////////

size_t Menge::Agents::ConstantPersistentAgentGenerator::agentCount() { return _count; }

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ConstantPersistentAgentGenerator::setAgentPosition(size_t i, BaseAgent* agt) {
  agt->_pos = addNoise(Menge::Vector2(_posX, _posY));
}

//////////////////////////////////////////////////////////////////////////////

bool Menge::Agents::ConstantPersistentAgentGenerator::shouldGenerate(float time_step) {
  _elapsedTime += time_step;
  if (_elapsedTime >= _spawnInterval) {
    _elapsedTime -= _spawnInterval;
    return true;
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////

size_t Menge::Agents::ConstantPersistentAgentGenerator::getSpawnRate() const { return _spawnRate; }

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ConstantPersistentAgentGenerator::setGeneratorPosition(float x, float y) {
  _posX = x;
  _posY = y;
}

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ConstantPersistentAgentGenerator::setSpawnRate(int rate) { _spawnRate = rate; }

//////////////////////////////////////////////////////////////////////////////

void Menge::Agents::ConstantPersistentAgentGenerator::setSpawnInterval(float interval) {
  _spawnInterval = interval;
}

void Menge::Agents::ConstantPersistentAgentGenerator::spawn() {}

//////////////////////////////////////////////////////////////////////////////
// 		Implementation of ConstantPersistentAgentGeneratorFactory
//////////////////////////////////////////////////////////////////////////////

bool Menge::Agents::ConstantPersistentAgentGeneratorFactory::setFromXML(
    PersistentAgentGenerator* gen, TiXmlElement* node, const std::string& behaveFldr) const {
  ConstantPersistentAgentGenerator* cpGen = dynamic_cast<ConstantPersistentAgentGenerator*>(gen);
  assert(cpGen != 0x0 &&
         "Trying to set attributes of a constant persistent agent generator component on an "
         "incompatible object");

  if (!PersistentAgentGeneratorFactory::setFromXML(cpGen, node, behaveFldr)) return false;

  double x_pos, y_pos, spawn_interval;
  int spawn_rate;

  if (node->Attribute("x_pos") && node->Attribute("y_pos") && node->Attribute("spawn_interval") &&
      node->Attribute("spawn_rate")) {
    node->Attribute("x_pos", &x_pos);
    node->Attribute("y_pos", &y_pos);
    node->Attribute("spawn_interval", &spawn_interval);
    node->Attribute("spawn_rate", &spawn_rate);
  } else {
    logger << Logger::ERR_MSG
           << "One of the required attributes (x_pos, y_pos, spawn_interval, "
              "spawn_rate) is missing for the ConstantPersistentAgentGenerator "
              "on line "
           << node->Row() << ".";
    return false;
  }

  cpGen->setGeneratorPosition(x_pos, y_pos);
  cpGen->setSpawnInterval(spawn_interval);
  cpGen->setSpawnRate(spawn_rate);

  return true;
}