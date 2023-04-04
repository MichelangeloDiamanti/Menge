#ifndef __CONSTANT_PERSISTENT_AGENT_GENERATOR_H__
#define __CONSTANT_PERSISTENT_AGENT_GENERATOR_H__

#include "MengeCore/Agents/AgentGenerators/Persistent/PersistentAgentGenerator.h"
#include "MengeCore/Agents/AgentGenerators/Persistent/PersistentAgentGeneratorFactory.h"
#include "MengeCore/mengeCommon.h"

namespace Menge {

namespace Agents {

class MENGE_API ConstantPersistentAgentGenerator : public PersistentAgentGenerator {
 public:
  // Inherited via PersistentAgentGenerator
  virtual size_t agentCount() override;
  virtual bool shouldGenerate(float time_step) override;
  virtual size_t getSpawnRate() const override;
  virtual void setAgentPosition(size_t i, BaseAgent* agt) override;

  void setGeneratorPosition(float x, float y);
  void setSpawnRate(int rate);
  void setSpawnInterval(float interval);

 protected:
  size_t _count;  // The number of agents that have been generated so far

  float _posX;           // x position of the AgentGenerator
  float _posY;           // y position of the AgentGenerator
  int _spawnRate;        // number of agents to spawn
  float _spawnInterval;  // time interval between spawns (seconds)
  float _elapsedTime;    // time elapsed since last spawn (seconds)
};

class MENGE_API ConstantPersistentAgentGeneratorFactory : public PersistentAgentGeneratorFactory {
 public:
  // Inherited via PersistentAgentGeneratorFactory
  virtual const char* name() const { return "persistent_constant"; }
  virtual const char* description() const override {
    return "A persistent agent generator that continuously spawns a fixed number of agents at a "
           "specified time interval";
  }

 protected:
  virtual PersistentAgentGenerator* instance() const override {
    return new ConstantPersistentAgentGenerator();
  }
  virtual bool setFromXML(PersistentAgentGenerator* gen, TiXmlElement* node,
                          const std::string& behaveFldr) const;
};

}  // namespace Agents
}  // namespace Menge
#endif  // __CONSTANT_PERSISTENT_AGENT_GENERATOR_H__
