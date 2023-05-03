#ifndef __EXTERNAL_PERSISTENT_AGENT_GENERATOR_H__
#define __EXTERNAL_PERSISTENT_AGENT_GENERATOR_H__

#include "MengeCore/Agents/AgentGenerators/Persistent/PersistentAgentGenerator.h"
#include "MengeCore/Agents/AgentGenerators/Persistent/PersistentAgentGeneratorFactory.h"
#include "MengeCore/mengeCommon.h"

namespace Menge {

namespace Agents {

class MENGE_API ExternalPersistentAgentGenerator : public PersistentAgentGenerator {
 public:

  struct PositionOrientation {
    Vector2 position;
    Vector2 orientation;
  };
     
  // Inherited via PersistentAgentGenerator
  virtual size_t agentCount() override;
  virtual bool shouldGenerate(float time_step) override;
  virtual size_t getSpawnRate() const override;
  virtual void spawn() override;
  virtual void setAgentPosition(size_t i, BaseAgent* agt) override;
  virtual void setAgentOrientation(size_t i, BaseAgent* agt) override;

  /*!
   @brief    Adds a position to the generator

   @param    p    The position to add.
   */
  void addPositionOrientation(const Vector2& p, const Vector2& o = Vector2(0.f, 1.f));
  void removePositionOrientation(const Vector2& p);
  void clearPositionOrientations();
  
 protected:
  size_t _count = 0;  // The number of agents that have been generated so far

  /*!
   @brief    The agent positions set from the external source.
   */
  std::vector<PositionOrientation> _posOrients;

  // Now I need to implement a mechanism which allows to trigger the spawning of agents
  // ideally, the external entity will call the spawn function, and there is going to be a 
  // variable which will be set to true when the spawn function is called
  // and then the shouldGenerate function will return true if the variable is true
  // and then the variable will be set to false
  
  size_t _positionsSet = 0;
};

class MENGE_API ExternalPersistentAgentGeneratorFactory : public PersistentAgentGeneratorFactory {
 public:
  // Inherited via PersistentAgentGeneratorFactory
  virtual const char* name() const { return "persistent_external"; }
  virtual const char* description() const override {
    return "A persistent agent generator that spawns agents on demand from an external source.";
  }

 protected:
  virtual PersistentAgentGenerator* instance() const override {
    return new ExternalPersistentAgentGenerator();
  }
  virtual bool setFromXML(PersistentAgentGenerator* gen, TiXmlElement* node,
                          const std::string& behaveFldr) const;
};

}  // namespace Agents
}  // namespace Menge
#endif  // __EXTERNAL_PERSISTENT_AGENT_GENERATOR_H__
