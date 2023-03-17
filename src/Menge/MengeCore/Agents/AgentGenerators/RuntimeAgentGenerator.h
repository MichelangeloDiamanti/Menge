#ifndef __RUNTIME_AGENT_GENERATOR_H__
#define __RUNTIME_AGENT_GENERATOR_H__

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/ProfileSelectors/ProfileSelector.h"

namespace Menge {

namespace Agents {

class RuntimeAgentGenerator {
 public:
  virtual ~RuntimeAgentGenerator() {}

  virtual bool shouldSpawn(float simTime, size_t& count) = 0;
  virtual void setAgentPosition(size_t i, BaseAgent* agt) = 0;

  virtual bool setAgentProfile(ProfileSelector*) const = 0;
  virtual ProfileSelector* getAgentProfile() const = 0;
};

}  // namespace Agents

}  // namespace Menge
#endif