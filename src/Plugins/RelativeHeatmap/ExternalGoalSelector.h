//#ifndef __EXTERNAL_GOAL_SELECTOR_H__
//#define __EXTERNAL_GOAL_SELECTOR_H__
//
//#include <chrono>
//#include <map>
//
//#include "MengeCore/BFSM/GoalSelectors/GoalSelector.h"
//#include "MengeCore/BFSM/GoalSelectors/GoalSelectorFactory.h"
//#include "MengeCore/BFSM/GoalSet.h"
//#include "MengeCore/BFSM/Goals/Goal.h"
//#include "MengeCore/BFSM/Goals/GoalPoint.h"
//#include "MengeCore/BFSM/Goals/GoalFactory.h"
//#include "MengeCore/BFSM/fsmCommon.h"
//#include "MengeCore/Runtime/os.h"
//#include "RelativeHeatmapConfig.h"
//#include "MengeCore/Runtime/ReadersWriterLock.h"
//
//namespace RelativeHeatmap {
//
//// forward declaration
//class ExternalGoalSelectorFactory;
//
///*!
// @brief    The explicit goal selector makes extracts a specific goal from a specific goal set --
// both explicitly stated.
// */
//class RELATIVE_HEATMAP_API ExternalGoalSelector : public Menge::BFSM::GoalSelector {
// public:
//  /*!
//   @brief    Default constructor
//   */
//  ExternalGoalSelector();
//
//  Menge::BFSM::Goal* getGoal(const Menge::Agents::BaseAgent* agent) const;
//
//  bool setGoal(const Menge::Agents::BaseAgent* agent, Menge::BFSM::PointGoal* goal);
//
//  bool setGoal(size_t agentId, Menge::BFSM::PointGoal* goal);
//
//  /*!
//   @brief    Gives the instance the opportunity to set the goal set.
//
//   This does nothing for instances which don't require a goal set. Otherwise, it extracts its
//   required goal set.
//
//   @param    goalSets  A mapping from goal set identifier to goal set pointers.
//   */
//  void setGoalSet(std::map<size_t, Menge::BFSM::GoalSet*>& goalSets);
//
//  /*!
//   @brief    Sets the goal set identifier.
//
//   @param    id    The goal set's id that belongs to this selector.
//   */
//  void setGoalSetID(size_t id) { _goalSetID = id; }
//
//  /*!
//   @brief    Sets the goal identifier.
//
//   @param    id    The goal's id that belongs to this selector.
//   */
//  void setGoalID(size_t id) { _goalID = id; }
//
//  /*!
//   @brief		how many goals has this selector instantiated
//  */
//  mutable int _goals_count = 0;
//  
//  /*!
//   @brief    A mapping from agent id to assigned goals.
//
//   This will only contain meaningful values in one of two cases:
//   - If the selector is persistent.
//   - If compiled in debug mode (and then node freeing will be tested against this map).
//   */
//  HASH_MAP<size_t, Menge::BFSM::Goal*> _agentsGoals;
//
// protected:
//  /*!
//   @brief    The id of the goal set to draw from.
//   */
//  size_t _goalSetID;
//
//  /*!
//   @brief    The goal associated with this goal selector.
//
//   During parsing, it contains the id of the goal. After FSM construction, it contains a pointer to
//   the actual goal.
//   */
//  union {
//    size_t _goalID;            ///< The identifier for the goal
//    Menge::BFSM::Goal* _goal;  ///< The pointer to the goal.
//  };
//
//  /*!
//   @brief    The lock to maintain readers-writer access to the _assignedGoals
//   */
//  Menge::ReadersWriterLock _lock;
//};
//
///*!
// @brief    Factory for the ExplicitGoalSelector.
// */
//class RELATIVE_HEATMAP_API ExternalGoalSelectorFactory : public Menge::BFSM::GoalSelectorFactory {
// public:
//  /*!
//   @brief    Constructor.
//   */
//  ExternalGoalSelectorFactory();
//
//  /*!
//   @brief    The name of the goal selector type.
//
//   The goal selector's name must be unique among all registered goal selectors. Each goal selector
//   factory must override this function.
//
//   @returns  A string containing the unique goal selector name.
//   */
//  virtual const char* name() const { return "external"; }
//
//  /*!
//   @brief    A description of the goal selector.
//
//   Each goal selector factory must override this function.
//
//   @returns  A string containing the goal selector description.
//   */
//  virtual const char* description() const {
//    return "A goal selector.  The goal an agent gets is "
//           "assigned by an external entity and just stored on a per-agent basis";
//  };
//
// protected:
//  /*!
//   @brief    Create an instance of this class's goal selector.
//
//   @returns    A pointer to a newly instantiated GoalSelector class.
//   */
//  Menge::BFSM::GoalSelector* instance() const { return new ExternalGoalSelector(); }
//
//  /*!
//   @brief    Given a pointer to a GoalSelector instance, sets the appropriate fields from the
//            provided XML node.
//
//   @param    selector    A pointer to the goal whose attributes are to be set.
//   @param    node        The XML node containing the goal selector attributes.
//   @param    behaveFldr  The path to the behavior file.  If the goal selector references resources
//   in the file system, it should be defined relative to the behavior file location. This is the
//   folder containing that path.
//   @returns  A boolean reporting success (true) or failure (false).
//   */
//  virtual bool setFromXML(Menge::BFSM::GoalSelector* selector, TiXmlElement* node,
//                          const std::string& behaveFldr) const;
//
//  /*!
//   @brief    The identifier for the "goal_set" size_t attribute.
//   */
//  size_t _goalSetID;
//
//  /*!
//   @brief    The identifier for the "goal" size_t attribute.
//   */
//  size_t _goalID;
//};
//
//}  // namespace RelativeHeatmap
//
//#endif  // __GOAL_SELECTOR_EXCPLICIT_H__
