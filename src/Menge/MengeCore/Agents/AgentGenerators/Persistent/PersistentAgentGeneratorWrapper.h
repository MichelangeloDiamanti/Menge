#ifndef __PERSISTENT_AGENT_GENERATOR_WRAPPER_H__
#define __PERSISTENT_AGENT_GENERATOR_WRAPPER_H__

#include "MengeCore/Agents/ProfileSelectors/ProfileSelectorDatabase.h"
#include "MengeCore/Agents/StateSelectors/StateSelectorDatabase.h"

namespace Menge {

namespace Agents {

class PersistentAgentGeneratorWrapper {
 public:
  PersistentAgentGeneratorWrapper(PersistentAgentGenerator* generator, ProfileSelector* profileSel,
                                  StateSelector* stateSel)
      : _generator(generator), _profileSelector(profileSel), _stateSelector(stateSel) {}

  PersistentAgentGenerator* getGenerator() const { return _generator; }
  ProfileSelector* getProfileSelector() const { return _profileSelector; }
  StateSelector* getStateSelector() const { return _stateSelector; }

 private:
  PersistentAgentGenerator* _generator;
  ProfileSelector* _profileSelector;
  StateSelector* _stateSelector;
};

}  // namespace Agents
}  // namespace Menge

#endif  // __PERSISTENT_AGENT_GENERATOR_WRAPPER_H__