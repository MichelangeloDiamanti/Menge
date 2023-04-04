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

#ifndef __SIMULATOR_BASE_H__
#define __SIMULATOR_BASE_H__

/*!
 @file      SimulatorBase.h
 @brief     Contains the SimulatorBase class - the common, generic simulator to work with different
            types of agents. It is templated on the Agent type.
 */

#include <vector>


#include "MengeCore/Agents/AgentInitializer.h"
#include "MengeCore/Agents/AgentGenerators/Persistent/PersistentAgentGenerator.h"
#include "MengeCore/Agents/AgentGenerators/Persistent/PersistentAgentGeneratorWrapper.h"
#include "MengeCore/Agents/SimulatorState.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Agents/SpatialQueries/SpatialQuery.h"
#include "MengeCore/Runtime/Utils.h"
#include "MengeCore/mengeCommon.h"

#if HAVE_OPENMP || _OPENMP
#include <omp.h>
#endif

namespace Menge {

namespace Agents {

/*!
 @brief      Defines the basic simulator. It is responsible for tracking agents and obstacles as
             well as initializing such from files.
 */
template <class Agent>
class SimulatorBase : public SimulatorInterface {
 public:
  /*!
   @brief      Constructs a simulator instance.
   */
  SimulatorBase();

  /*!
   @brief      Destorys a simulator instance.
   */
  ~SimulatorBase();

  /*!
   @brief     Lets the simulator perform a simulation step and updates the two-dimensional _p and
              two-dimensional velocity of each agent.
   */
  void doStep();

/*!
  @brief Generates and initializes new agents based on the registered PersistentAgentGenerators.

  This function iterates over the registered PersistentAgentGenerators in the simulator and checks
  if it is time to generate new agents based on each generator's internal conditions. For each
  generator that meets the generation conditions, it creates new agents using the provided agent
  profile, sets their initial positions and states, initializes them, and registers them for all
  velocity modifiers.
  */
  void GenerateAgents();

  /*!
   @brief    Initalize spatial query structure.
   */
  virtual bool initSpatialQuery();

  /*!
   @brief    After all agents and all obstacles have been added to the scene does the work to finish
            preparing the simulation to be run.

   This work is performed when the simulator is done being initialized. If a particular new
   pedestrian simulator requires particular finalization work, this function should be sub-classed
   and the parent class's version of the function should be explicitly called before any additional
   work is performed.
   */
  virtual void finalize();

  /*!
   @brief      Accessor for agents.

   @param      agentNo      The number of the agent who is to be retrieved. This is *not* the
                            same as the agent identifier. It is merely the local index of the agent
                            in the simulator's local store.
   @returns    A pointer to the agent.
   */
  virtual BaseAgent* getAgent(size_t agentNo) { return &_agents[agentNo]; }

  /*!
   @brief      Const accessor for agents.

   @param      agentNo    The number of the agent who is to be retrieved. This is *not* the same
                          as the agent identifier. It is merely the local index of the agent in the
                          simulator's local store.
   @returns    A pointer to the agent.
   */
  virtual const BaseAgent* getAgent(size_t agentNo) const { return &_agents[agentNo]; }

  // Inherited via SimulatorInterface
  virtual void addPersistentGeneratorMapping(PersistentAgentGenerator* generator,
                                   PersistentAgentGeneratorWrapper* wrapper) override;


  /*!
   @brief    Add an agent with specified position to the simulator whose properties are defined by
            the given agent initializer. It uses the agent initializer to define the values of the
   remaining agent parameters.

   @param    pos          The 2d vector representing the agent's position.
   @param    agentInit    The AgentInitializer necessary to parse AgentSet properties.
   @returns  A pointer to the agent (if initialization was succesful) or NULL if failed.
   */
  virtual BaseAgent* addAgent(const Vector2& pos, AgentInitializer* agentInit);

  /*!
   @brief      Returns the count of agents in the simulation.

   @returns    The count of agents in the simulation.
   */
  virtual size_t getNumAgents() const { return _agents.size(); }

  /*!
   @brief      Reports if there are non-common Experiment parameters that this simulator requires in
              the XML file.

   @returns    By default, the simulator base ONLY uses common parameters. Always returns false.
   */
  virtual bool hasExpTarget() { return false; }

  /*!
   @brief      Reports if the given Experiment attribute tag name belongs to this simulator.

   @param      tagName      The name of the candidate experiment XML tag.
   @returns    By default, the simulator base ONLY uses common parameters. Always returns false.
   */
  virtual bool isExpTarget(const std::string& tagName) { return false; }

  /*!
   @brief      Given an Experiment parameter name and value, sets the appropriate simulator
              parameter.

   // TODO: Define the conditions of success/failure.

   @param      paramName    A string containing the parameter name for the experiment.
   @param      value        A string containing the value for the parameter.
   @returns    True if the parameter was successfully set, false otherwise.
   */
  virtual bool setExpParam(const std::string& paramName,
                           const std::string& value) throw(XMLParamException);

 protected:
  /*!
   @brief    Computes the neighbors for the given agent.

   @param    agent    The agent whose neighbors are to be computed.
   */
  void computeNeighbors(Agent* agent);

  /*!
   @brief    The collection of agents in the simulation
   */
  std::vector<Agent> _agents;

  std::unordered_map<PersistentAgentGenerator*, PersistentAgentGeneratorWrapper*> _generatorMap;

};


}  // namespace Agents
}  // namespace Menge
#endif  // __SIMULATOR_BASE_H__
