#ifndef __ABSOLUTE_HEATMAP_GOAL_H__
#define __ABSOLUTE_HEATMAP_GOAL_H__

#include "AbsoluteHeatmap.h"
#include "MengeCore/BFSM/GoalSet.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/Goals/GoalFactory.h"
#include "MengeCore/BFSM/fsmCommon.h"
#include "MengeCore/Runtime/os.h"
#include "RelativeHeatmapConfig.h"

#include <chrono>

namespace RelativeHeatmap {

// forward declaration
class AbsoluteHeatmapGoalFactory;

/*!
 @brief    A simple point goal.  The goal is trivially this point.
 */
class RELATIVE_HEATMAP_API AbsoluteHeatmapGoal : public Menge::BFSM::Goal {
 public:
  /*! Default constructor */
  AbsoluteHeatmapGoal();

  virtual ~AbsoluteHeatmapGoal();

  /*!
   @brief    Full constructor.

   @param    p    The position of the goal.
   */
  AbsoluteHeatmapGoal(const Menge::Math::Vector2& p);

  /*!
   @brief    Full constructor - component-wise.

   @param    x    The x-position of the goal.
   @param    y    The x-position of the goal.
   */
  AbsoluteHeatmapGoal(float x, float y);

  /*!
   @brief    Used by the plugin system to know what artifacts to associate with agents of this type.

   Every sub-class of must return a globally unique value if it should be associated with unique
   artifacts.
   */
  virtual std::string getStringId() const { return NAME; }

  
  
  void assign(const Menge::Agents::BaseAgent* agent);

  void free();
  
  /*!
  @brief		Set the AbsoluteHeatmap data.
  @param		AbsoluteHeatmap		A managed resource pointer to the underlying
  AbsoluteHeatmap data.
  */
  void setAbsoluteHeatmap(AbsoluteHeatmapPtr AbsoluteHeatmap);

  AbsoluteHeatmapPtr getAbsoluteHeatmap() { return _absoluteHeatmap; };

  /*
      @brief iterates over the heatmap, get the highest value, and return the point as a goal
  */
  Menge::Math::Vector2 getGoalPosition(const Menge::Agents::BaseAgent* agent);

  /*! The unique identifier used to register this type with run-time components. */
  static const std::string NAME;

  /*!
   @brief		The underlying AbsoluteHeatmap data.
   */
  AbsoluteHeatmapPtr _absoluteHeatmap;
  
  /*!
   @brief		how far (ardoun the agent) to look for the goal. If its -1 then the whole map will be scanned.
   */
  float _look_radius;
};

/*!
 @brief    Factory for the PointGoal.
 */
class RELATIVE_HEATMAP_API AbsoluteHeatmapGoalFactory : public Menge::BFSM::GoalFactory {
 public:
  AbsoluteHeatmapGoalFactory();

  /*!
   @brief    The name of the goal type.

   The goal's name must be unique among all registered goals. Each goal factory must override this
   function.

   @returns  A string containing the unique goal name.
   */
  virtual const char* name() const { return AbsoluteHeatmapGoal::NAME.c_str(); }

  /*!
   @brief    A description of the goal.

   Each goal factory must override this function.

   @returns  A string containing the goal description.
   */
  virtual const char* description() const {
    return "An agent goal consisting of a single point in two-dimensional space";
  };

 protected:
  /*!
   @brief    Create an instance of this class's goal.

   @returns    A pointer to a newly instantiated Goal class.
   */
  Menge::BFSM::Goal* instance() const { return new AbsoluteHeatmapGoal(); }

  /*!
   @brief    Given a pointer to a Goal instance, sets the appropriate fields from the provided XML
            node.

   @param    goal        A pointer to the goal whose attributes are to be set.
   @param    node        The XML node containing the action attributes.
   @param    behaveFldr  The path to the behavior file.  If the action references resources in the
                        file system, it should be defined relative to the behavior file location.
                        This is the folder containing that path.
   @returns  A boolean reporting success (true) or failure (false).
   */
  virtual bool setFromXML(Menge::BFSM::Goal* goal, TiXmlElement* node,
                          const std::string& behaveFldr) const;

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

#endif  // __GOAL_POINT_H__
