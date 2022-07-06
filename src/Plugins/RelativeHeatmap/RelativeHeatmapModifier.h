#ifndef __RELATIVE_HEATMAP_MODIFIER_H__
#define __RELATIVE_HEATMAP_MODIFIER_H__

#include "RelativeHeatmap.h"
#include "RelativeHeatmapConfig.h"

#include "MengeCore/BFSM/VelocityModifiers/VelModifier.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifierFactory.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Runtime/os.h"
#include "thirdParty/tinyxml.h"

#define cimg_use_png
#include "CImg.h"
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
class RelativeHeatmapModifierFactory;

class RELATIVE_HEATMAP_API RelativeHeatmapModifier : public Menge::BFSM::VelModifier {
 public:
  /*!
   @brief	Constructor
   */
  RelativeHeatmapModifier();

 protected:
  /*!
   @brief		Destructor.
   */
  virtual ~RelativeHeatmapModifier();

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

  /*!
  @brief		Set the relativeHeatmap data.
  @param		relativeHeatmap		A managed resource pointer to the underlying
  relativeHeatmap data.
  */
  void setRelativeHeatmap(RelativeHeatmapPtr relativeHeatmap);

  RelativeHeatmapPtr getRelativeHeatmap() { return _relativeHeatmap; };

  friend class RelativeHeatmapModifierFactory;

 protected:
  /*!
   @brief		The underlying relativeHeatmap data.
   */
  RelativeHeatmapPtr _relativeHeatmap;

  /*!
   @brief		The scale factor between heatmap pixels and world units.
				Ex. if it is 0.1, it means that 1 pixel is 0.1 world units
   */
  float _scale;

  /*!
   @brief		The offset, from the center of the heatmap, where the agent is considered
   being.
   */
  Menge::Vector2 _offset;
};

///////////////////////////////////////////////////////////////////////////////

/*!
 @brief		The factory class for the RelativeHeatmapModifier
 */
class RELATIVE_HEATMAP_API RelativeHeatmapModifierFactory : public Menge::BFSM::VelModFactory {
 public:
  /*!
   @brief		Constructor.
   */
  RelativeHeatmapModifierFactory();

  /*!
   @brief		The name of the modifier

   The modifier's name must be unique among all registered modifier.
   Each modifier factory must override this function.

   @returns	A string containing the unique modifier name.
   */
  virtual const char* name() const { return "relative_heatmap"; }

  /*!
   @brief		A description of the modifier

   Each modifier factory must override this function.

   @returns	A string containing the modifier description.
   */
  virtual const char* description() const {
    return "Sets agents velocity according to a relativeHeatmap: spots with \"hot colors\" are "
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
  Menge::BFSM::VelModifier* instance() const { return new RelativeHeatmapModifier(); }

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
#endif  // __RELATIVE_HEATMAP_MODIFIER_H__