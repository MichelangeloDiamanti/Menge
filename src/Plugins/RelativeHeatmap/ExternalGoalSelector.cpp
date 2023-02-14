//#include "ExternalGoalSelector.h"
//
//#include <cassert>
//
//#include "MengeCore/Agents/BaseAgent.h"
//#include "MengeCore/Agents/SimulatorInterface.h"
//#include "MengeCore/BFSM/GoalSet.h"
//#include "MengeCore/BFSM/Goals/Goal.h"
//#include "MengeCore/BFSM/Goals/GoalPoint.h"
//#include "MengeCore/Core.h"
//
//using namespace Menge;
//using namespace Menge::BFSM;
//using namespace Menge::Agents;
//
//using namespace std;
//
//namespace RelativeHeatmap {
///////////////////////////////////////////////////////////////////////
////                   Implementation of ExternalGoalSelector
///////////////////////////////////////////////////////////////////////
//
//ExternalGoalSelector::ExternalGoalSelector() : GoalSelector() {}
//
///////////////////////////////////////////////////////////////////////
//
//Goal* ExternalGoalSelector::getGoal(const BaseAgent* agent) const {
//  Goal* goal = 0x0;
//
//  _lock.lockRead();
//  HASH_MAP<size_t, Goal*>::const_iterator itr = _agentsGoals.find(agent->_id);
//  if (itr != _agentsGoals.end()) {
//    goal = itr->second;
//  }
//  _lock.releaseRead();
//
//  if (goal) return goal;
//}
//
//bool ExternalGoalSelector::setGoal(const Menge::Agents::BaseAgent* agent,
//                                   Menge::BFSM::PointGoal* goal) {
//  _lock.lockWrite();
//  HASH_MAP<size_t, Goal*>::iterator itr = _agentsGoals.find(agent->_id);
//  if (itr != _agentsGoals.end()) {
//    itr->second = goal;
//    return true;
//  }
//  _lock.releaseWrite();
//
//  return false;
//}
//
//bool ExternalGoalSelector::setGoal(size_t agentId, Menge::BFSM::PointGoal* goal) {
//  bool res = false;
//
//  const size_t NUM_AGENT = Menge::SIMULATOR->getNumAgents();
//  for (size_t i = 0; i < NUM_AGENT; ++i) {
//    const BaseAgent* testAgent = Menge::SIMULATOR->getAgent(i);
//    if (testAgent->_id == agentId) {
//      res = setGoal(testAgent, goal);
//    }
//  }
//
//  return res;
//}
//
///////////////////////////////////////////////////////////////////////
//
//void ExternalGoalSelector::setGoalSet(std::map<size_t, GoalSet*>& goalSets) {
//  if (goalSets.count(_goalSetID) == 1) {
//    GoalSet* gs = goalSets[_goalSetID];
//    size_t gid = _goalID;
//    _goal = gs->getGoalByID(gid);
//    if (_goal == 0x0) {
//      std::stringstream ss;
//      logger << Logger::ERR_MSG << "Goal Selector cannot find targeted goal (";
//      logger << gid << ") in desired goal set (" << _goalSetID << ").";
//      throw GoalSelectorException();
//    }
//  } else {
//    logger << Logger::ERR_MSG;
//    logger << "Explicit goal selector tried accessing a goal set that doesn't exist: ";
//    logger << _goalSetID << "\n";
//    throw GoalSelectorException();
//  }
//}
//
///////////////////////////////////////////////////////////////////////
////                   Implementation of ExternalGoalSelectorFactory
///////////////////////////////////////////////////////////////////////
//
//ExternalGoalSelectorFactory::ExternalGoalSelectorFactory() : GoalSelectorFactory() {
//  _goalSetID = _attrSet.addSizeTAttribute("goal_set", true /*required*/);
//  _goalID = _attrSet.addSizeTAttribute("goal", true /*required*/);
//}
//
///////////////////////////////////////////////////////////////////////
//
//bool ExternalGoalSelectorFactory::setFromXML(GoalSelector* selector, TiXmlElement* node,
//                                             const std::string& behaveFldr) const {
//  ExternalGoalSelector* gs = dynamic_cast<ExternalGoalSelector*>(selector);
//  assert(gs != 0x0 && "Trying to set external goal selector attributes on an incompatible object.");
//
//  if (!GoalSelectorFactory::setFromXML(gs, node, behaveFldr)) return false;
//
//  gs->setGoalSetID(_attrSet.getSizeT(_goalSetID));
//  gs->setGoalID(_attrSet.getSizeT(_goalID));
//
//  gs->setGoal(size_t(0), new PointGoal(268.0f, -147.0f));
//
//  return true;
//}
//}  // namespace RelativeHeatmap
