/*
 Menge Crowd Simulation Framework

 Copyright and trademark 2012-17 University of North Carolina at Chapel Hill

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
 or
    LICENSE.txt in the root of the Menge repository.

 Any questions or comments should be sent to the authors menge@cs.unc.edu

 <http://gamma.cs.unc.edu/Menge/>
*/

/*!
 @file    AgentGenerator.h
 @brief   The definition of the agent generator element. Defines the intial numbers and positions
          of agents in the simulation.
 */
#ifndef __PERSISTENT_AGENT_GENERATOR_H__
#define __PERSISTENT_AGENT_GENERATOR_H__

#include "MengeCore/Agents/AgentGenerators/AgentGenerator.h"

namespace Menge {

namespace Agents {

class MENGE_API PersistentAgentGenerator : public AgentGenerator {
 public:
  virtual bool shouldGenerate(float time_step) = 0;

  virtual size_t getSpawnRate() const = 0;
};

}  // namespace Agents
}  // namespace Menge
#endif  // __PERSISTENT_AGENT_GENERATOR_H__
