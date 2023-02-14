/*

License

Menge
Copyright © and trademark ™ 2012-14 University of North Carolina at Chapel Hill.
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation
for educational, research, and non-profit purposes, without fee, and without a
written agreement is hereby granted, provided that the above copyright notice,
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North
Carolina at Chapel Hill. The software program and documentation are supplied "as is,"
without any accompanying services from the University of North Carolina at Chapel
Hill or the authors. The University of North Carolina at Chapel Hill and the
authors do not warrant that the operation of the program will be uninterrupted
or error-free. The end-user understands that the program was developed for research
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

#include <cassert>

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorExternal.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/Goal.h"

namespace Menge {

namespace BFSM {
/////////////////////////////////////////////////////////////////////
//                   Implementation of ExplicitGoalSelector
/////////////////////////////////////////////////////////////////////

ExternalGoalSelector::ExternalGoalSelector() : GoalSelector() {}

/////////////////////////////////////////////////////////////////////

Goal* ExternalGoalSelector::getGoal(const Agents::BaseAgent* agent) const {
  Goal* goal = 0x0;

  _lock.lockRead();
  HASH_MAP<size_t, Goal*>::const_iterator itr = _agentsGoals.find(agent->_id);
  if (itr != _agentsGoals.end()) {
    goal = itr->second;
  }
  _lock.releaseRead();

  if (goal != 0x0) return goal;
  
  //return new PointGoal(agent->_pos);
  return new PointGoal(78.0f, -169.0f);
}

/////////////////////////////////////////////////////////////////////

bool ExternalGoalSelector::setGoal(const Menge::Agents::BaseAgent* agent,
                                   Menge::BFSM::PointGoal* goal) {
  bool res = false;
  _lock.lockWrite();
  HASH_MAP<size_t, Goal*>::iterator itr = _agentsGoals.find(agent->_id);
  if (itr != _agentsGoals.end()) {
    itr->second = goal;
    res = true;
  } else {
    _agentsGoals.insert(std::pair<size_t, Goal*>(agent->_id, goal));
    res = false;
  }
  _lock.releaseWrite();

  return res;
}

/////////////////////////////////////////////////////////////////////

bool ExternalGoalSelector::setGoal(size_t agentId, Menge::BFSM::PointGoal* goal) {
  bool res = false;
  _lock.lockWrite();
  HASH_MAP<size_t, Goal*>::iterator itr = _agentsGoals.find(agentId);
  if (itr != _agentsGoals.end()) {
    itr->second = goal;
    res = true;
  } else {
    _agentsGoals.insert(std::pair<size_t, Goal*>(agentId, goal));
    res = false;
  }
  _lock.releaseWrite();

  return res;
}

/////////////////////////////////////////////////////////////////////

void ExternalGoalSelector::setGoalSet(std::map<size_t, GoalSet*>& goalSets) {
  if (goalSets.count(_goalSetID) == 1) {
    GoalSet* gs = goalSets[_goalSetID];
    size_t gid = _goalID;
    _goal = gs->getGoalByID(gid);
    if (_goal == 0x0) {
      std::stringstream ss;
      logger << Logger::ERR_MSG << "Goal Selector cannot find targeted goal (";
      logger << gid << ") in desired goal set (" << _goalSetID << ").";
      throw GoalSelectorException();
    }
  } else {
    logger << Logger::ERR_MSG;
    logger << "Explicit goal selector tried accessing a goal set that doesn't exist: ";
    logger << _goalSetID << "\n";
    throw GoalSelectorException();
  }
}

/////////////////////////////////////////////////////////////////////
//                   Implementation of ExplicitGoalSelectorFactory
/////////////////////////////////////////////////////////////////////

ExternalGoalSelectorFactory::ExternalGoalSelectorFactory() : GoalSelectorFactory() {
  _goalSetID = _attrSet.addSizeTAttribute("goal_set", true /*required*/);
  _goalID = _attrSet.addSizeTAttribute("goal", true /*required*/);
}

/////////////////////////////////////////////////////////////////////

bool ExternalGoalSelectorFactory::setFromXML(GoalSelector* selector, TiXmlElement* node,
                                             const std::string& behaveFldr) const {
  ExternalGoalSelector* gs = dynamic_cast<ExternalGoalSelector*>(selector);
  assert(gs != 0x0 && "Trying to set explicit goal selector attributes on an incompatible object.");

  if (!GoalSelectorFactory::setFromXML(gs, node, behaveFldr)) return false;

  gs->setGoalSetID(_attrSet.getSizeT(_goalSetID));
  gs->setGoalID(_attrSet.getSizeT(_goalID));

  return true;
}
}  // namespace BFSM
}  // namespace Menge
