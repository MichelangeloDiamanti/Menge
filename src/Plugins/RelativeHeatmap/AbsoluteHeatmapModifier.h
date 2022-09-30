#ifndef __ABSOLUTE_HEATMAP_MODIFIER_H__
#define __ABSOLUTE_HEATMAP_MODIFIER_H__

#define _USE_MATH_DEFINES

#include "AbsoluteHeatmap.h"
#include "RelativeHeatmapConfig.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/SpatialQueries/SpatialQueryStructs.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifier.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifierFactory.h"
#include "MengeCore/Runtime/os.h"
#include "thirdParty/tinyxml.h"

#define cimg_use_png
#include <map>
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
class AbsoluteHeatmapModifierFactory;

class RELATIVE_HEATMAP_API AbsoluteHeatmapModifier : public Menge::BFSM::VelModifier {
 public:
  /*!
   @brief	Constructor
   */
  AbsoluteHeatmapModifier();

 protected:
  /*!
   @brief		Destructor.
   */
  virtual ~AbsoluteHeatmapModifier();

 public:
  /*!
  @brief		Creates a copy of this velocity modifier.
  */
  Menge::BFSM::VelModifier* copy() const;

  /*!
   @brief       Registers an agent for use with the VelModifier.
   @param       agent    The agent to be registered
   */
  void registerAgent(const Menge::Agents::BaseAgent* agent);

  /*!
   @brief       Registers an agent for use with the VelModifier.
   @param       agent    The agent to be unregistered
   */
  void unregisterAgent(const Menge::Agents::BaseAgent* agent);

  /*!
   @brief		Adapt the input preferred velocity according to the relative heatmap.

   @param		agent		The agent whose preferred velocity is provided.
   @param		pVel		The preferred velocity to modify -- modified in place.
   */
  void adaptPrefVelocity(const Menge::Agents::BaseAgent* agent, Menge::Agents::PrefVelocity& pVel);

  int scoreRGBColor(int* color);

  float scoreAngle(Vector2 agentOrientation, Vector2 velocityOrientation);

  /*!
  @brief		Set the AbsoluteHeatmap data.
  @param		AbsoluteHeatmap		A managed resource pointer to the underlying
  AbsoluteHeatmap data.
  */
  void setAbsoluteHeatmap(AbsoluteHeatmapPtr AbsoluteHeatmap);

  AbsoluteHeatmapPtr getAbsoluteHeatmap() { return _absoluteHeatmap; };

  friend class AbsoluteHeatmapModifierFactory;

 protected:
  /*!
   @brief		The underlying AbsoluteHeatmap data.
   */
  AbsoluteHeatmapPtr _absoluteHeatmap;

  float _visionAngle = 120.0;  // degrees of the vision angle
  float _visionSampleAngleRate =
      10.0;                  // each "n" degree the map will be sampled at the vision range distance
  float _visionRange = 5.0;  // how far the agent sees (world units)

  Vector2 _heatmapSubGoal;
  bool _hasSubGoal = false;
  float _minDistanceToSubGoal = 0.1f;

  bool _traceBack = false;

  Vector2 _preferredVelocityMovingPoint;

  /*!
   @brief		weight of the RED component of the color when sampling the map.
   */
  float _redWeight = 3.0f;

  /*!
   @brief		weight of the GREEN component of the color when sampling the map.
   */
  float _greenWeight = 2.0f;

  /*!
   @brief		weight of the BLUE component of the color when sampling the map.
   */
  float _blueWeight = 1.0f;
};

///////////////////////////////////////////////////////////////////////////////

/*!
 @brief		The factory class for the AbsoluteHeatmapModifier
 */
class RELATIVE_HEATMAP_API AbsoluteHeatmapModifierFactory : public Menge::BFSM::VelModFactory {
 public:
  /*!
   @brief		Constructor.
   */
  AbsoluteHeatmapModifierFactory();

  /*!
   @brief		The name of the modifier

   The modifier's name must be unique among all registered modifier.
   Each modifier factory must override this function.

   @returns	A string containing the unique modifier name.
   */
  virtual const char* name() const { return "absolute_heatmap"; }

  /*!
   @brief		A description of the modifier

   Each modifier factory must override this function.

   @returns	A string containing the modifier description.
   */
  virtual const char* description() const {
    return "Sets agents velocity according to a AbsoluteHeatmap: spots with \"hot colors\" are "
           "more likely "
           "to be selected";
  };

 protected:
  /*!
   @brief		Create an instance of this class's modifier.

   All modifierFactory sub-classes must override this by creating (on the heap) a new instance of
   its corresponding modifier type. The various field values of the instance will be set in a
   subsequent call to modifierFactory::setFromXML(). The caller of this function takes ownership of
   the memory.

   @returns		A pointer to a newly instantiated modifier class.
   */
  Menge::BFSM::VelModifier* instance() const { return new AbsoluteHeatmapModifier(); }

  /*!
   @brief		Given a pointer to an modifier instance, sets the appropriate fields from
   the provided XML node.

   It is assumed that the value of the `type` attribute is this modifier's type (i.e.
   modifierFactory::thisFactory() has already been called and returned true). If sub-classes of
   ModifierFactory introduce *new* modifier parameters, then the sub-class should override this
   method but explicitly call the parent class's version.

   @param		modifier	  A pointer to the modifier whose attributes are to be set.
   @param		node		    The XML node containing the modifier attributes.
   @param		behaveFldr	The path to the behavior file. If the modifier references
   resources in the file system, it should be defined relative to the behavior file location. This
   is the folder containing that path.
   @returns	A boolean reporting success (true) or failure (false).
   */
  virtual bool setFromXML(Menge::BFSM::VelModifier* modifier, TiXmlElement* node,
                          const std::string& behaveFldr) const;

  /*!
   @brief		The identifier for the "file_name" string attribute.
   */
  size_t _fileNameID;

  /*!
   @brief		The identifier for the "scale" float attribute.
   */
  size_t _scaleID;
};

}  // namespace RelativeHeatmap
#endif  // __RELATIVE_HEATMAP_MODIFIER_H__