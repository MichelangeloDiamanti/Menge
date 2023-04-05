#include "MengeCore/menge_c_api.h"

#include <algorithm>

#include "MengeCore/Agents/AgentGenerators/Persistent/ExternalPersistentAgentGenerator.h"
#include "MengeCore/Agents/AgentGenerators/Persistent/PersistentAgentGeneratorWrapper.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/Events/EventSystem.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Agents/SpatialQueries/SpatialQuery.h"
#include "MengeCore/BFSM/FSM.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelector.h";
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorExternal.h";
#include "MengeCore/BFSM/Goals/Goal.h";
#include "MengeCore/BFSM/Goals/GoalPoint.h";
#include "MengeCore/BFSM/State.h"
#include "MengeCore/Core.h"
#include "MengeCore/PluginEngine/CorePluginEngine.h"
#include "MengeCore/Runtime/SimulatorDB.h"
// #include "Plugins/RelativeHeatmap/ExternalGoalSelector.h"

/////////////////////////////////////////////////////////////////////
//          Local Variables
/////////////////////////////////////////////////////////////////////

Menge::Agents::SimulatorInterface* _simulator = 0x0;

Menge::PluginEngine::CorePluginEngine* _engine = 0x0;

/////////////////////////////////////////////////////////////////////
//          API implementation
/////////////////////////////////////////////////////////////////////

extern "C" {

using Menge::Agents::BaseAgent;
using Menge::Agents::Obstacle;
using Menge::Math::Vector2;
using std::find;

bool InitSimulator(const char* behaveFile, const char* sceneFile, const char* model,
                   const char* pluginPath) {
  const bool VERBOSE = false;
  // if (_simulator != 0x0) delete _simulator;
  Menge::SimulatorDB simDB;
  // TODO: Plugin engine is *not* public.  I can't get plugins.
  // if (_engine != 0x0) delete _engine;
  _engine = new Menge::PluginEngine::CorePluginEngine(&simDB);
  if (pluginPath != 0x0) {
    _engine->loadPlugins(pluginPath);
  }
  Menge::SimulatorDBEntry* simDBEntry = simDB.getDBEntry(std::string(model));
  if (simDBEntry == 0x0) return false;

  size_t agentCount;
  float timeStep = 0.1f;        // Default to 10Hz
  int subSteps = 0;             // take no sub steps
  float duration = 1e6;         // effectively no simulation duration.
  std::string outFile = "";     // Don't write an scb file.
  std::string scbVersion = "";  // No scb version
  bool verbose = false;
  _simulator = simDBEntry->getSimulator(agentCount, timeStep, subSteps, duration, behaveFile,
                                        sceneFile, outFile, scbVersion, verbose);
  return _simulator != 0x0;
}

/////////////////////////////////////////////////////////////////////

void SubscribeToAgentChangedStateEvent(
    Menge::BFSM::AgentChangedStateCallback agentChangedStateCallbackFunction) {
  Menge::BFSM::FSM* bfsm = _simulator->getBFSM();
  // bfsm->agentChangedStateCallbackFunction = agentChangedStateCallbackFunction;
  //   iterates the nodes of the FSM
  for (size_t i = 0; i < bfsm->getNodeCount(); i++) {
    Menge::BFSM::State* state = bfsm->getNode(i);
    state->agentChangedStateCallbackFunction = agentChangedStateCallbackFunction;
  }
}

/////////////////////////////////////////////////////////////////////

MENGE_API bool AddPositionToExternalAgentGenerator(const char* generatorName, float x, float y) {
  assert(_simulator != nullptr);

  Menge::Agents::PersistentAgentGeneratorWrapper* wrapper =
      _simulator->getPersistentGeneratorWrapper(generatorName);
  if (wrapper == 0x0) return false;

  Menge::Agents::ExternalPersistentAgentGenerator* extGenerator =
      dynamic_cast<Menge::Agents::ExternalPersistentAgentGenerator*>(wrapper->getGenerator());
  if (extGenerator == 0x0) return false;

  Menge::Math::Vector2 position = Menge::Math::Vector2(x, y);

  extGenerator->addPosition(position);
  return true;
}

/////////////////////////////////////////////////////////////////////

MENGE_API bool ClearExternalAgentGeneratorPositions(const char* generatorName) {
  assert(_simulator != nullptr);

  Menge::Agents::PersistentAgentGeneratorWrapper* wrapper =
      _simulator->getPersistentGeneratorWrapper(generatorName);
  if (wrapper == 0x0) return false;

  Menge::Agents::ExternalPersistentAgentGenerator* extGenerator =
      dynamic_cast<Menge::Agents::ExternalPersistentAgentGenerator*>(wrapper->getGenerator());
  if (extGenerator == 0x0) return false;

  extGenerator->clearPositions();
  return true;
}

/////////////////////////////////////////////////////////////////////

MENGE_API bool TriggerExternalAgentGeneratorSpawn(const char* generatorName) {
  assert(_simulator != nullptr);

  Menge::Agents::PersistentAgentGeneratorWrapper* wrapper =
      _simulator->getPersistentGeneratorWrapper(generatorName);
  if (wrapper == 0x0) return false;

  Menge::Agents::ExternalPersistentAgentGenerator* extGenerator =
      dynamic_cast<Menge::Agents::ExternalPersistentAgentGenerator*>(wrapper->getGenerator());
  if (extGenerator == 0x0) return false;

  extGenerator->spawn();
  return true;
}

/////////////////////////////////////////////////////////////////////

size_t UnloadPlugins() {
  size_t unloadedPlugins = _engine->unloadPlugins();
  return unloadedPlugins;
}

/////////////////////////////////////////////////////////////////////

void SetTimeStep(float timeStep) {
  assert(_simulator != 0x0);
  _simulator->setTimeStep(timeStep);
}

/////////////////////////////////////////////////////////////////////

bool DoStep() {
  assert(_simulator != 0x0);
  // if (gCBF != 0x0) gCBF();
  return _simulator->step();
}

/////////////////////////////////////////////////////////////////////

const char* GetStateName(size_t state_id) {
  assert(_simulator != nullptr);
  Menge::BFSM::FSM* bfsm = _simulator->getBFSM();
  if (state_id < bfsm->getNodeCount()) {
    Menge::BFSM::State* state = bfsm->getNode(state_id);
    return state->getName().c_str();
  }
  return nullptr;
}

/////////////////////////////////////////////////////////////////////

size_t StateCount() {
  assert(_simulator != nullptr);
  Menge::BFSM::FSM* bfsm = _simulator->getBFSM();
  return bfsm->getNodeCount();
}

/////////////////////////////////////////////////////////////////////

size_t AgentCount() {
  assert(_simulator != 0x0);
  return _simulator->getNumAgents();
}

/////////////////////////////////////////////////////////////////////

bool GetAgentPosition(size_t i, float* x, float* y, float* z) {
  assert(_simulator != 0x0);
  BaseAgent* agt = _simulator->getAgent(i);
  if (agt != 0x0) {
    *x = agt->_pos._x;
    *y = _simulator->getElevation(agt);
    *z = agt->_pos._y;
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////

bool GetAgentVelocity(size_t i, float* x, float* y, float* z) {
  assert(_simulator != 0x0);
  BaseAgent* agt = _simulator->getAgent(i);
  if (agt != 0x0) {
    *x = agt->_vel._x;
    *y = 0;  // get elevation
    *z = agt->_vel._y;
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////

bool GetAgentPrefVelocity(size_t i, float* x, float* y) {
  assert(_simulator != nullptr);
  Menge::Agents::BaseAgent* agt = _simulator->getAgent(i);
  if (agt != nullptr) {
    const auto& vel_pref = agt->_velPref.getPreferredVel();
    *x = vel_pref._x;
    *y = vel_pref._y;
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////

bool GetAgentState(size_t i, size_t* state_id) {
  assert(_simulator != nullptr);
  Menge::Agents::BaseAgent* agt = _simulator->getAgent(i);
  if (agt != nullptr) {
    const auto* bfsm = _simulator->getBFSM();
    *state_id = bfsm->getAgentStateID(agt->_id);
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////

bool GetAgentOrient(size_t i, float* x, float* y) {
  assert(_simulator != 0x0);
  BaseAgent* agt = _simulator->getAgent(i);
  if (agt != 0x0) {
    *x = agt->_orient._x;
    *y = agt->_orient._y;
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////

int GetAgentClass(size_t i) {
  assert(_simulator != 0x0);
  BaseAgent* agt = _simulator->getAgent(i);
  if (agt != 0x0) {
    return static_cast<int>(agt->_class);
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////

float GetAgentRadius(size_t i) {
  assert(_simulator != 0x0);
  BaseAgent* agt = _simulator->getAgent(i);
  if (agt != 0x0) {
    return agt->_radius;
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////

std::vector<std::string> triggers;
bool triggersValid = false;

/////////////////////////////////////////////////////////////////////

int ExternalTriggerCount() {
  if (!triggersValid) triggers = Menge::EVENT_SYSTEM->listExternalTriggers();
  return static_cast<int>(triggers.size());
}

/////////////////////////////////////////////////////////////////////

const char* ExternalTriggerName(int i) {
  if (i < ExternalTriggerCount()) {
    return triggers[i].c_str();
  }
  return nullptr;
}

/////////////////////////////////////////////////////////////////////

void FireExternalTrigger(const char* triggerName) {
  Menge::EVENT_SYSTEM->activateExternalTrigger(triggerName);
}

/////////////////////////////////////////////////////////////////////

size_t ObstacleCount() {
  assert(_simulator != 0x0);
  return _simulator->getSpatialQuery()->getObstacles().size();
}

/////////////////////////////////////////////////////////////////////

size_t GetNextObstacle(size_t i) {
  assert(_simulator != 0x0);
  const std::vector<Obstacle*>& obstacles = _simulator->getSpatialQuery()->getObstacles();
  assert(i < obstacles.size());
  const Obstacle* queryObstacle = obstacles[i];
  const Obstacle* nextObstacle = queryObstacle->next();
  std::vector<Obstacle*>::const_iterator itr =
      find(obstacles.begin(), obstacles.end(), nextObstacle);
  assert(itr != obstacles.end());
  return itr - obstacles.begin();
}

/////////////////////////////////////////////////////////////////////

bool GetObstacleEndPoints(size_t i, float* x0, float* y0, float* z0, float* x1, float* y1,
                          float* z1) {
  assert(_simulator != 0x0);
  const std::vector<Obstacle*>& obstacles = _simulator->getSpatialQuery()->getObstacles();
  assert(i < obstacles.size());
  const Obstacle* queryObstacle = obstacles[i];
  const Vector2& p0 = queryObstacle->getP0();
  *x0 = p0._x;
  *y0 = 0.0;  // TODO: Use elevation to set this more intelligently.
  *z0 = p0._y;
  const Vector2& p1 = queryObstacle->getP1();
  *x1 = p1._x;
  *y1 = 0.0;  // TODO: Use elevation to set this more intelligently.
  *z1 = p1._y;
  return true;
}

/////////////////////////////////////////////////////////////////////

bool GetObstacleP0(size_t i, float* x0, float* y0, float* z0) {
  assert(_simulator != 0x0);
  const std::vector<Obstacle*>& obstacles = _simulator->getSpatialQuery()->getObstacles();
  assert(i < obstacles.size());
  const Obstacle* queryObstacle = obstacles[i];
  const Vector2& p0 = queryObstacle->getP0();
  *x0 = p0._x;
  *y0 = 0.0;  // TODO: Use elevation to set this more intelligently.
  *z0 = p0._y;
  return true;
}

/////////////////////////////////////////////////////////////////////

bool GetObstacleP1(size_t i, float* x1, float* y1, float* z1) {
  assert(_simulator != 0x0);
  const std::vector<Obstacle*>& obstacles = _simulator->getSpatialQuery()->getObstacles();
  assert(i < obstacles.size());
  const Obstacle* queryObstacle = obstacles[i];
  const Vector2& p1 = queryObstacle->getP1();
  *x1 = p1._x;
  *y1 = 0.0;  // TODO: Use elevation to set this more intelligently.
  *z1 = p1._y;
  return true;
}

/////////////////////////////////////////////////////////////////////

MENGE_API int TestNewFunction() { return 44; }

/////////////////////////////////////////////////////////////////////

MENGE_API bool GetAgentGoal(size_t i, size_t* goal_id) {
  assert(_simulator != nullptr);
  Menge::Agents::BaseAgent* agt = _simulator->getAgent(i);
  if (agt != nullptr) {
    const auto* bfsm = _simulator->getBFSM();
    *goal_id = bfsm->getCurrentState(agt)->getGoalSelector()->getGoal(agt)->getID();

    return true;
  }
  return false;
}

MENGE_API bool IsStateGoalSelectorExternal(const char* stateName) {
  assert(_simulator != nullptr);

  bool res = false;
  const auto* bfsm = _simulator->getBFSM();

  Menge::BFSM::State* s = bfsm->getState(stateName);
  if (s != 0x0) {
    Menge::BFSM::GoalSelector* selector = s->getGoalSelector();

    if (selector != 0x0) {
      // try casting to ExternalGoalSelector. If it works, the goal selector is external
      Menge::BFSM::ExternalGoalSelector* gs =
          dynamic_cast<Menge::BFSM::ExternalGoalSelector*>(selector);

      if (gs != 0x0) res = true;
    }
  }

  return res;
}

MENGE_API bool SetAgentPointGoal(size_t agentId, float x, float y) {
  assert(_simulator != nullptr);

  bool res = false;

  Menge::Agents::BaseAgent* agt = _simulator->getAgent(agentId);
  if (agt != nullptr) {
    const auto* bfsm = _simulator->getBFSM();
    Menge::BFSM::GoalSelector* selector = bfsm->getCurrentState(agt)->getGoalSelector();
    Menge::BFSM::ExternalGoalSelector* gs =
        dynamic_cast<Menge::BFSM::ExternalGoalSelector*>(selector);
    // assert(gs != 0x0 &&
    //        "Trying to cast an incompatible object to ExternalGoalSelector.");

    if (gs != 0x0) {
      gs->freeGoal(agt, gs->getGoal(agt));
      res = gs->setGoal(agentId, new Menge::BFSM::PointGoal(x, y));
      gs->assignGoal(agt);
    }
  }
  return res;
}

MENGE_API bool SetStatePointGoalForAgent(const char* stateName, size_t agentId, float x, float y) {
  assert(_simulator != nullptr);
  bool res = false;
  Menge::BFSM::FSM* bfsm = _simulator->getBFSM();
  if (bfsm != nullptr) {
    Menge::BFSM::State* state = bfsm->getState(stateName);
    Menge::BFSM::GoalSelector* selector = bfsm->getState(stateName)->getGoalSelector();
    Menge::BFSM::ExternalGoalSelector* gs =
        dynamic_cast<Menge::BFSM::ExternalGoalSelector*>(selector);

    Menge::Agents::BaseAgent* agt = _simulator->getAgent(agentId);

    if (gs != 0x0 && agt != 0x0) {
      res = gs->setGoal(agentId, new Menge::BFSM::PointGoal(x, y));
      // calling the enter allows the stateselctor to assign the goal
      // state->enter(agt);
    }
  }
  return res;
}

}  // extern"C"