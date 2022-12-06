#ifndef __ABSOLUTE_HEATMAP_GOAL_H__
#define __ABSOLUTE_HEATMAP_GOAL_H__

#define _USE_MATH_DEFINES

#include <chrono>

#include "AbsoluteHeatmap.h"
#include "MengeCore/BFSM/Goals/Goal.h"
#include "MengeCore/BFSM/Goals/GoalFactory.h"
#include "RelativeHeatmapConfig.h"
#include "thirdParty/tinyxml.h"

#define cimg_use_png

#include "CImg.h"
#include "math.h"
#include "png.h"

#if defined(_MSC_VER)
// Visual Studio spews warnings about some members.
// It wants them to be exposed in the dll IN CASE a client uses it.
// TODO: remove this warning supression, correct the problem
#pragma warning(disable : 4251)
#endif

// forward declaration
class TiXmlElement;

namespace RelativeHeatmap {

// forward declaration
class AbsoluteHeatmapGoalFactory;

class RELATIVE_HEATMAP_API AbsoluteHeatmapGoal : public Menge::BFSM::Goal {
 public:
  /*! Default constructor */
  AbsoluteHeatmapGoal();

  /*!
       @brief    Full constructor.

       @param    heatmap    The heatmap of the goal.
   */
  AbsoluteHeatmapGoal(const AbsoluteHeatmap& heatmap);

  /*!
   @brief    Used by the plugin system to know what artifacts to associate with agents of this type.

   Every sub-class of must return a globally unique value if it should be associated with unique
   artifacts.
   */
  virtual std::string getStringId() const { return NAME; }

  /*! The unique identifier used to register this type with run-time components. */
  static const std::string NAME;
};

/*!
 @brief    Factory for the AbsoluteHeatmapGoal.
 */
class MENGE_API AbsoluteHeatmapGoalFactory : public Menge::BFSM::GoalFactory {
 public:
	
  // constructor
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
    return "An agent goal consisting of a single point based on the brightest pixel in a heatmap.";
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
};

}  // namespace RelativeHeatmap

#endif