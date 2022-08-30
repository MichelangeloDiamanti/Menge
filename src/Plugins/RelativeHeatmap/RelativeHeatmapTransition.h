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
 @file		AircraftTransition.h
 @brief		Definition of actions used in aircraft: loading and unloading.
 */
#ifndef __RELATIVE_HEATMAP_TRANSITION_H__
#define __RELATIVE_HEATMAP_TRANSITION_H__

#include "RelativeHeatmap.h"
#include "RelativeHeatmapConfig.h"

#include <map>
#include "MengeCore/BFSM/FSMEnumeration.h"
#include "MengeCore/BFSM/Transitions/Condition.h"
#include "MengeCore/BFSM/Transitions/ConditionFactory.h"
#include "MengeCore/Math/Geometry2D.h"

#include "MengeCore/Runtime/os.h"
#include "thirdParty/tinyxml.h"

// forward declaration
class TiXmlElement;

namespace RelativeHeatmap {
// forward declaration
class ColorConditionFactory;

/*!
 @brief		An action that sets agent properties based on the agent's position along the x-axis.
 */
class RELATIVE_HEATMAP_API ColorCondition : public Menge::BFSM::Condition {
 public:
  /*!
   @brief		Constructor
   */
  ColorCondition();

  /*!
   @brief		Copy constructor

   @param		cond		The condition to copy from.
   */
  ColorCondition(const ColorCondition& cond);

 protected:
  /*!
   @brief		Virtual destructor.
   */
  virtual ~ColorCondition();

 public:
  /*!
   @brief		The AutoCondition is always met -- it is a tautology.

   @param		agent		The agent to test the transition for.
   @param		goal		The agent's goal (although this may be ignored).
   @returns	True if the condition has been met.
   */
  virtual bool conditionMet(Menge::Agents::BaseAgent* agent, const Menge::BFSM::Goal* goal);

  /*!
   @brief		Create a copy of this condition.

   It is the responsibility of the caller to delete the object.

   @returns:	A "deep copy" of this condition - such that there is no shared objects between this
              and its copy.
   */
  virtual Menge::BFSM::Condition* copy();

  /*!
  @brief		Set the relativeHeatmap data.
  @param		relativeHeatmap		A managed resource pointer to the underlying
  relativeHeatmap data.
  */
  void setRelativeHeatmap(RelativeHeatmapPtr relativeHeatmap);

  friend class ColorConditionFactory;

 protected:
  /*!
   @brief		The underlying relativeHeatmap data.
   */
  RelativeHeatmapPtr _relativeHeatmap;

  /*!
   @brief		The RGB color which will trigger this condition.
   */
  int* _conditionColorRGB;
};

/*!
 @brief		The factory for creating the ColorCondition
 */
class RELATIVE_HEATMAP_API ColorConditionFactory : public Menge::BFSM::ConditionFactory {
 public:
  /**
   * Default construcotr for the factory
   */
  ColorConditionFactory();

  /*!
   @brief		The name of the action.

   The action's name must be unique among all registered actions. Each action factory must override
   this function.

   @returns	A string containing the unique action name.
   */
  const char* name() const { return "match_color"; }

  /*!
   @brief		A description of the action.

   Each action factory must override this function.

   @returns	A string containing the action description.
   */
  const char* description() const {
    return "The color condition evaluates to true if any of the neighbouring agents, projected to "
           "the heatmap, returns an RGB value which matches the color";
  }

 protected:
  /*!
   @brief		Create an instance of this class's condition.

   All ConditionFactory sub-classes must override this by creating (on the heap) a new instance of
   its corresponding condition type. The various field values of the instance will be set in a
   subsequent call to ConditionFactory::setFromXML(). The caller of this function takes ownership of
   the memory.

   @returns		A pointer to a newly instantiated Action class.
   */
  ColorCondition* instance() const { return new ColorCondition(); }

  /*!
   @brief		Given a pointer to an Condition instance, sets the appropriate fields from
   the provided XML node.

   It is assumed that the value of the `type` attribute is this Tarnsitions's type (i.e.
   ConditionFactory::thisFactory() has already been called and returned true). If sub-classes of
   ConditionFactory introduce *new* Condition parameters, then the sub-class should override this
   method but explicitly call the parent class's version.

   @param		condition		  A pointer to the condition whose attributes are to
   be set.
   @param		node			    The XML node containing the condition
   attributes.
   @param		behaveFldr		The path to the behavior file. If the condition
   references resources in the file system, it should be defined relative to the behavior file
   location. This is the folder containing that path.
   @returns	True if successful.
   */
  virtual bool setFromXML(Menge::BFSM::Condition* condition, TiXmlElement* node,
                          const std::string& behaveFldr) const;

  /*!
   @brief		The identifier for the "file_name" string attribute.
   */
  size_t _fileNameID;

  /*!
   @brief		the RGB color which will trigger this transition.
   */
  size_t _colorRGB;

  /*!
   @brief		The identifier for the "scale" float attribute.
   */
  size_t _scaleID;

  /*!
   @brief		The identifier for the "offset_x" float attribute.
  */
  size_t _offsetXID;

  /*!
   @brief		The identifier for the "offset_y" float attribute.
  */
  size_t _offsetYID;
};

}  // namespace RelativeHeatmap

#endif
