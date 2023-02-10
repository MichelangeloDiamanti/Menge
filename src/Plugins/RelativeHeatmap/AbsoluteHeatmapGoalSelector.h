#ifndef __ABSOLUTE_HEATMAP_GOAL_SELECTOR_H__
#define __ABSOLUTE_HEATMAP_GOAL_SELECTOR_H__

#include <chrono>

#include "AbsoluteHeatmap.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelector.h"
#include "MengeCore/BFSM/GoalSelectors/GoalSelectorFactory.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/Goals/GoalFactory.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/Runtime/os.h"
#include "RelativeHeatmapConfig.h"

namespace RelativeHeatmap {

// forward declaration
class AbsoluteHeatmapGoalFactory;

/*!
 @brief    The explicit goal selector makes extracts a specific goal from a specific goal set --
 both explicitly stated.
 */
class RELATIVE_HEATMAP_API AbsoluteHeatmapGoalSelector : public Menge::BFSM::GoalSelector {
 public:
  /*!
   @brief    Default constructor
   */
  AbsoluteHeatmapGoalSelector();

  Menge::BFSM::Goal* getGoal(const Menge::Agents::BaseAgent* agent) const;

  float scoreGoal(int x, int y, const Menge::Agents::BaseAgent* agent, int* rgb) const;

  /*!
   @brief    Gives the instance the opportunity to set the goal set.

   This does nothing for instances which don't require a goal set. Otherwise, it extracts its
   required goal set.

   @param    goalSets  A mapping from goal set identifier to goal set pointers.
   */
  void setGoalSet(std::map<size_t, Menge::BFSM::GoalSet*>& goalSets);

  void setAbsoluteHeatmap(AbsoluteHeatmapPtr absoluteHeatmap);

  AbsoluteHeatmapPtr getAbsoluteHeatmap() { return _absoluteHeatmap; };

  /*!
   @brief    Sets the goal set identifier.

   @param    id    The goal set's id that belongs to this selector.
   */
  void setGoalSetID(size_t id) { _goalSetID = id; }

  /*!
   @brief    Sets the goal identifier.

   @param    id    The goal's id that belongs to this selector.
   */
  void setGoalID(size_t id) { _goalID = id; }

  /*!
   @brief		The underlying AbsoluteHeatmap data.
   */
  AbsoluteHeatmapPtr _absoluteHeatmap;

  /*!
   @brief		how far (ardoun the agent) to look for the goal. If its -1 then the whole
   map will be scanned.
   */
  float _look_radius;

  /*!
   @brief		how many goals has this selector instantiated
  */
  mutable int _goals_count = 0;

 protected:
  /*!
   @brief    The id of the goal set to draw from.
   */
  size_t _goalSetID;

  /*!
   @brief    The goal associated with this goal selector.

   During parsing, it contains the id of the goal. After FSM construction, it contains a pointer to
   the actual goal.
   */
  union {
    size_t _goalID;            ///< The identifier for the goal
    Menge::BFSM::Goal* _goal;  ///< The pointer to the goal.
  };
};

/*!
 @brief    Factory for the ExplicitGoalSelector.
 */
class RELATIVE_HEATMAP_API AbsoluteHeatmapGoalSelectorFactory
    : public Menge::BFSM::GoalSelectorFactory {
 public:
  /*!
   @brief    Constructor.
   */
  AbsoluteHeatmapGoalSelectorFactory();

  /*!
   @brief    The name of the goal selector type.

   The goal selector's name must be unique among all registered goal selectors. Each goal selector
   factory must override this function.

   @returns  A string containing the unique goal selector name.
   */
  virtual const char* name() const { return "absolute_heatmap"; }

  /*!
   @brief    A description of the goal selector.

   Each goal selector factory must override this function.

   @returns  A string containing the goal selector description.
   */
  virtual const char* description() const {
    return "A goal selector.  The goal an agent gets is "
           "computed based on an heatmap.";
  };

 protected:
  /*!
   @brief    Create an instance of this class's goal selector.

   @returns    A pointer to a newly instantiated GoalSelector class.
   */
  Menge::BFSM::GoalSelector* instance() const { return new AbsoluteHeatmapGoalSelector(); }

  /*!
   @brief    Given a pointer to a GoalSelector instance, sets the appropriate fields from the
            provided XML node.

   @param    selector    A pointer to the goal whose attributes are to be set.
   @param    node        The XML node containing the goal selector attributes.
   @param    behaveFldr  The path to the behavior file.  If the goal selector references resources
   in the file system, it should be defined relative to the behavior file location. This is the
   folder containing that path.
   @returns  A boolean reporting success (true) or failure (false).
   */
  virtual bool setFromXML(Menge::BFSM::GoalSelector* selector, TiXmlElement* node,
                          const std::string& behaveFldr) const;

  /*!
   @brief    The identifier for the "goal_set" size_t attribute.
   */
  size_t _goalSetID;

  /*!
   @brief    The identifier for the "goal" size_t attribute.
   */
  size_t _goalID;

  /*!
   @brief		The identifier for the "file_name" string attribute.
   */
  size_t _fileNameID;

  /*!
   @brief		The identifier for the "scale" float attribute.
   */
  size_t _scaleID;

  /*!
   @brief		The identifiers for the "offset" Vector2 attribute.
   */
  size_t _offsetXID;
  size_t _offsetYID;

  /*!
   @brief		The identifier for the "look_radius" float attribute.
   */
  size_t _look_radiusID;
};

}  // namespace RelativeHeatmap

#endif  // __GOAL_SELECTOR_EXCPLICIT_H__
