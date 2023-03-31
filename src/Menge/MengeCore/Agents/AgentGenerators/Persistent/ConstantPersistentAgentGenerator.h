#ifndef __CONSTANT_PERSISTENT_AGENT_GENERATOR_H__
#define __CONSTANT_PERSISTENT_AGENT_GENERATOR_H__

#include "MengeCore/Agents/AgentGenerators/Persistent/PersistentAgentGenerator.h"

namespace Menge {

namespace Agents {

class MENGE_API ConstantPersistentAgentGenerator : public PersistentAgentGenerator {
 public:
  // Inherited via PersistentAgentGenerator
  virtual size_t agentCount() override;
  virtual void setAgentPosition(size_t i, BaseAgent* agt) override;
  virtual bool shouldGenerate() const override;
};

}  // namespace Agents
}  // namespace Menge
#endif  // __CONSTANT_PERSISTENT_AGENT_GENERATOR_H__
