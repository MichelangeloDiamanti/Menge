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
 @file    SimSystem.h
 @brief   The system which runs the simulation, coordinating the FSM and simulator.
 */

#ifndef __SIM_SYSTEM_H__
#define __SIM_SYSTEM_H__

#include <unordered_map>
#include <vector>

#include "MengeCore/MengeException.h"
#include "MengeVis/SceneGraph/GLNode.h"
#include "MengeVis/SceneGraph/System.h"
#include "MengeVis/Runtime/MengeContext.h"


// forward declaration
namespace Menge {
namespace Agents {
class SimulatorInterface;
}
}  // namespace Menge

namespace MengeVis {
// forward declarations
namespace SceneGraph {
class GLScene;
}
namespace Runtime {
class VisAgent;

// Exceptions for the system
/*!
 @brief   Generic exception for the SimSystem.
 */
class MENGEVIS_API SimSystemException : public virtual Menge::MengeException {
 public:
  /*!
   @brief   Default constructor.
   */
  SimSystemException() : Menge::MengeException() {}

  /*!
   @brief   Constructor with message.

   @param   s   The exception-specific message.
   */
  SimSystemException(const std::string& s) : Menge::MengeException(s) {}
};

/*!
 @brief   The fatal SimSystem exception.
 */
class MENGEVIS_API SimSystemFatalException : public SimSystemException,
                                             public Menge::MengeFatalException {
 public:
  /*!
   @brief   Default constructor.
   */
  SimSystemFatalException()
      : Menge::MengeException(), SimSystemException(), Menge::MengeFatalException() {}

  /*!
   @brief   Constructor with message.

   @param   s   The exception-specific message.
   */
  SimSystemFatalException(const std::string& s)
      : Menge::MengeException(s), SimSystemException(), Menge::MengeFatalException() {}
};

/*!
 @brief   Templated class for performing simulation.

 The SimSystem is the main mechanism which performs the simulation loop. It is responsible for
 updating the simulation step after step, with appropriate calls to the BFSM and the simulator.
 */
class MENGEVIS_API SimSystem : public SceneGraph::System {
 public:
  /*!
   @brief   Constructor

   @param   sim   A pointer to the simulator.
   */
  SimSystem(Menge::Agents::SimulatorInterface* sim, SceneGraph::GLScene* scene);

  /*!
   @brief    Destructor.
   */
  ~SimSystem();

  virtual void setScene(SceneGraph::GLScene* scene);

  virtual void setMengeContext(MengeVis::Runtime::MengeContext* ctx);

  /*!
   @brief   Update the simulation (and possibly visual elements) to the given global time.

   @param   time    The global time of the system.
   @returns True if the system has changed such that it requires a redraw.
   */
  virtual bool updateScene(float time);

  /*!
   @brief   Updates the scene agent collection by synchronizing the _visAgents vector with the
            simulator's agent list.

   This function is called to ensure that the _visAgents vector accurately represents the current
   list of agents in the simulator. As the simulator adds agents, it might recreate its internal
   agent list to accommodate more agents, causing the memory addresses (pointers) of the agents to
   change. Consequently, it is necessary to clear the _agentRoot and _visAgents each time this
   function is called, to make sure they remain synchronized with the simulator's agent list.

   Instead of updating the existing nodes one by one to match the simulator's new agent list, it is
   more convenient to delete the previous nodes and create a new list of nodes that directly
   corresponds to the updated list of agents in the simulator. This approach simplifies the
   synchronization process and helps avoid potential issues with outdated or invalid pointers.
   */
  virtual bool updateSceneAgentCollection();

  /*!
   @brief   Add visual representations of the simulation obstcles to the GLScene.

   @param   scene   The scene which receives nodes for drawing obstacles.
   */
  void addObstacleToScene();

  /*!
   @brief   Add visual representations of the simulation agents to the GLScene.

   @param   scene   The scene which receives nodes for drawing agents.
   */
  virtual void addAgentsToScene();

  /*!
   @brief   Add visual representations of obstacles and agents to the GLScene.

   @param   scene   The scene which receives nodes for drawing agents.
   */
  void populateScene();

  /*!
   @brief   Update the position of the *visual* agents from the simulation data.

   @param   agtCount    The number of agents in the system.
   */
  virtual void updateAgentPosition(int agtCount);

  /*!
   @brief   Returns a pointer to the visualization agents

   @returns Returns the pointer to the pointers.
   */
  // inline VisAgent** getVisAgents() { return _visAgents; }

  /*!
   @brief   Reports the number of agents.

   @returns The number of VisAgents updated by the system.
   */
  size_t getAgentCount() const;

 protected:
  /*!
   @brief   Simulator to run and (possibly) visualize.
   */
  Menge::Agents::SimulatorInterface* _sim;

  /*!
   @brief   The visualization agents the system is responsible for updating.
   */
  // VisAgent** _visAgents;
  std::vector<VisAgent*> _visAgents;

  // size_t _visAgentCount;

  /*!
   @brief   The global time of last system update.
   */
  float _lastUpdate;

  /*!
   @brief   Indicates if the simulation is running.
   */
  bool _isRunning;

  SceneGraph::GLScene* _scene;
  
  MengeVis::Runtime::MengeContext* _mengeContext;

  SceneGraph::GLDagNode* _agentRoot;
  bool _isAgentSelected = false;
  size_t _selectedAgentId;
  SceneGraph::GLDagNode* _obstacleRoot;
};
}  // namespace Runtime
}  // namespace MengeVis
#endif  // __SIM_SYSTEM_H__
