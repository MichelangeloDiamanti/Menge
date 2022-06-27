#ifndef __TEXTURE_MODIFIER_H__
#define __TEXTURE_MODIFIER_H__
#endif

#include "TexturesConfig.h"

#include "MengeCore/BFSM/VelocityModifiers/VelModifier.h"
#include "MengeCore/BFSM/VelocityModifiers/VelModifierFactory.h"

#if defined(_MSC_VER)
// Visual Studio spews warnings about some members.
// It wants them to be exposed in the dll IN CASE a client uses it.
// TODO: remove this warning supression, correct the problem
#pragma warning(disable : 4251)
#endif

// forward declaration
class TiXmlElement;

namespace Texture {

// forward declaration
class TextureModifierFactory;

class TEXTURES_API TextureModifier : public Menge::BFSM::VelModifier {
 public:
  /*!
   @brief	Constructor
   */
  TextureModifier();

 protected:
  /*!
   @brief		Destructor.
   */
  virtual ~TextureModifier();

 public:
  /*!
  @brief		Creates a copy of this velocity modifier.
  */
  Menge::BFSM::VelModifier* copy() const;

  /*!
   @brief		Adapt the input preferred velocity according to the formation.

   @param		agent		The agent whose preferred velocity is provided.
   @param		pVel		The preferred velocity to modify -- modified in place.
   */
  void adaptPrefVelocity(const Menge::Agents::BaseAgent* agent, Menge::Agents::PrefVelocity& pVel);
};

///////////////////////////////////////////////////////////////////////////////

/*!
 @brief		The factory class for the TextureModifier
 */
class TEXTURES_API TextureModifierFactory : public Menge::BFSM::VelModFactory {
 public:
  /*!
   @brief		Constructor.
   */
  TextureModifierFactory();

  /*!
   @brief		The name of the modifier

   The modifier's name must be unique among all registered modifier.
   Each modifier factory must override this function.

   @returns	A string containing the unique modifier name.
   */
  virtual const char* name() const { return "texture"; }

  /*!
   @brief		A description of the modifier

   Each modifier factory must override this function.

   @returns	A string containing the modifier description.
   */
  virtual const char* description() const {
    return "Sets agents velocity according to a texture: spots with \"hot colors\" are more likely "
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
  Menge::BFSM::VelModifier* instance() const { return new TextureModifier(); }

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
};

}  // namespace Texture