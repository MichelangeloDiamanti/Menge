#include "MengeCore/Agents/SimulatorBase.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/State.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifier.h"
#include "MengeCore/PedVO/PedVOAgent.h"
#include "MengeCore/Orca/ORCAAgent.h"

namespace Menge {
namespace Agents {

////////////////////////////////////////////////////////////////
//          Implementation of SimulatorBase
////////////////////////////////////////////////////////////////

// Explicit instantiation of the template class for ORCA and PEDVO agents
template class Menge::Agents::SimulatorBase<ORCA::Agent>;
template class Menge::Agents::SimulatorBase<PedVO::Agent>;

template <class Agent>
SimulatorBase<Agent>::SimulatorBase() : SimulatorInterface(), _agents() {}

////////////////////////////////////////////////////////////////

template <class Agent>
SimulatorBase<Agent>::~SimulatorBase() {
  _agents.clear();
}

////////////////////////////////////////////////////////////////

template <class Agent>
void SimulatorBase<Agent>::doStep() {
  assert(_spatialQuery != 0x0 && "Can't run without a spatial query instance defined");

  // Iterate over the generator map and spawn agents
  for (auto& entry : _generatorMap) {
    AgentGenerator* generator = entry.first;
    PersistentAgentGeneratorWrapper* wrapper = entry.second;

    //// Check if it's time to spawn an agent based on your conditions, e.g., elapsed time, agent
    //// count, etc.
    // if (shouldSpawnAgent(generator)) {
    //  Retrieve the profile selector and state selector from the wrapper
    ProfileSelector* profileSel = wrapper->getProfileSelector();
    StateSelector* stateSel = wrapper->getStateSelector();

    // Now instantiate the agents
    const size_t AGT_COUNT = generator->agentCount();
    Vector2 zero;
    for (size_t i = 0; i < AGT_COUNT; ++i) {
      AgentInitializer* agentProfile = profileSel->getProfile();
      BaseAgent* agent = addAgent(zero, agentProfile);
      generator->setAgentPosition(i, agent);
      this->getInitialState()->setAgentState(agent->_id, stateSel->getState());
      
      this->getBFSM()->updateAgentCount(getNumAgents());
      _agents[agent->_id].initialize();

      BFSM::State* initialState = this->getBFSM()->getState(stateSel->getState().c_str());
      this->getBFSM()->setCurrentState(agent, initialState->getID());
      initialState->enter(agent);
      agent->_vel.set(Vector2(0.f, 0.f));
      
      // register the agent for all vel modifiers
      const std::vector<BFSM::VelModifier*>& velModifiers = this->getBFSM()->getVelocityModifiers();
      for (auto vItr = velModifiers.cbegin(); vItr != velModifiers.cend(); ++vItr) {
        (*vItr)->registerAgent(agent);
      }

    }

    // Perform any other required actions for the newly spawned agent
    //}
  }

  _spatialQuery->updateAgents();
  int AGT_COUNT = static_cast<int>(_agents.size());
#pragma omp parallel for
  for (int i = 0; i < AGT_COUNT; ++i) {
    computeNeighbors(&(_agents[i]));
    _agents[i].computeNewVelocity();
  }

#pragma omp parallel for
  for (int i = 0; i < AGT_COUNT; ++i) {
    _agents[i].update(TIME_STEP);
  }

  _globalTime += TIME_STEP;
}

////////////////////////////////////////////////////////////////

template <class Agent>
bool SimulatorBase<Agent>::initSpatialQuery() {
  assert(_spatialQuery != 0x0 && "Can't run without a spatial query instance defined");

  const size_t AGT_COUNT = _agents.size();
  std::vector<BaseAgent*> agtPointers(AGT_COUNT);
  for (size_t a = 0; a < AGT_COUNT; ++a) {
    agtPointers[a] = &_agents[a];
  }
  _spatialQuery->setAgents(agtPointers);

  _spatialQuery->processObstacles();

  return true;
}

////////////////////////////////////////////////////////////////

template <class Agent>
void SimulatorBase<Agent>::finalize() {
  SimulatorInterface::finalize();

  // initialize agents
  for (size_t i = 0; i < _agents.size(); ++i) {
    _agents[i].initialize();
  }
}

////////////////////////////////////////////////////////////////

template <class Agent>
inline void SimulatorBase<Agent>::addGeneratorMapping(AgentGenerator* generator,
                                                      PersistentAgentGeneratorWrapper* wrapper) {
  _generatorMap[generator] = wrapper;
}

template <class Agent>
BaseAgent* SimulatorBase<Agent>::addAgent(const Vector2& pos, AgentInitializer* agentInit) {
  if (agentInit != 0x0) {
    Agent agent;

    agent._pos = pos;
    agent._id = _agents.size();
    if (!agentInit->setProperties(&agent)) {
      logger << Logger::ERR_MSG << "Error initializing agent " << agent._id << "\n";
      return 0x0;
    }
    _agents.push_back(agent);
  } else {
    logger << Logger::ERR_MSG << "agentInit null\n";
  }
  return &_agents[_agents.size() - 1];
}

////////////////////////////////////////////////////////////////

template <class Agent>
bool SimulatorBase<Agent>::setExpParam(const std::string& paramName,
                                       const std::string& value) throw(XMLParamException) {
  if (paramName == "time_step") {
    try {
      LOGICAL_TIME_STEP = toFloat(value);
    } catch (UtilException) {
      throw XMLParamException(
          std::string("Common parameters \"time_step\" value couldn't be converted "
                      "to a float.  Found the value: ") +
          value);
    }
  } else {
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////

template <class Agent>
void SimulatorBase<Agent>::computeNeighbors(Agent* agent) {
  // obstacles
  agent->startQuery();
  _spatialQuery->obstacleQuery(agent);

  // agents
  if (agent->_maxNeighbors > 0) {
    _spatialQuery->agentQuery(agent);
  }
}
}  // namespace Agents
}  // namespace Menge