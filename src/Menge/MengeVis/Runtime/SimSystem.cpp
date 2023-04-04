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
#define NOMINMAX  // prevent windows.h from stomping on "min"
#include "MengeVis/Runtime/SimSystem.h"

#include <set>

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Agents/Obstacle.h"
#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Agents/SpatialQueries/SpatialQuery.h"
#include "MengeCore/Runtime/Logger.h"
#include "MengeVis/Runtime/AgentContext/BaseAgentContext.h"
#include "MengeVis/Runtime/VisAgent/VisAgent.h"
#include "MengeVis/Runtime/VisAgent/VisAgentDatabase.h"
#include "MengeVis/Runtime/VisObstacle.h"
#include "MengeVis/SceneGraph/GLScene.h"

namespace MengeVis {
namespace Runtime {

using Menge::logger;
using Menge::Logger;
using Menge::Agents::BaseAgent;
using Menge::Agents::Obstacle;
using Menge::Agents::SCBWriter;
using Menge::Agents::SimulatorInterface;
using Menge::Math::Vector2;
using Menge::Math::Vector3;
using SceneGraph::GLScene;
using SceneGraph::SystemStopException;

////////////////////////////////////////////////////////////////////////////
//      Implementation of SimSystem
////////////////////////////////////////////////////////////////////////////

SimSystem::SimSystem(SimulatorInterface* sim, GLScene* scene)
    : SceneGraph::System(), _sim(sim), _lastUpdate(0.f), _isRunning(true), _scene(scene) {}

////////////////////////////////////////////////////////////////////////////

SimSystem::~SimSystem() {
  // Clean up the VisAgent objects
  for (VisAgent* visAgent : _visAgents) {
    delete visAgent;
  }
  _visAgents.clear();

  if (_sim) delete _sim;
}

////////////////////////////////////////////////////////////////////////////

void SimSystem::setScene(SceneGraph::GLScene* scene) { _scene = scene; }

////////////////////////////////////////////////////////////////////////////

void SimSystem::setMengeContext(MengeVis::Runtime::MengeContext* ctx) { _mengeContext = ctx; }

////////////////////////////////////////////////////////////////////////////

bool SimSystem::updateScene(float time) {
  // TODO: I tried to do this in the updateSceneAgentCollection() function so it would be self
  // contained and not use member variables, but for some reason it doesnt work and the selected
  // agent is null
  VisAgent* selectedAgent = dynamic_cast<VisAgent*>(SceneGraph::Selectable::getSelectedObject());
  if (selectedAgent != 0x0) {
    _isAgentSelected = true;
    _selectedAgentId = selectedAgent->getAgent()->_id;
  } else {
    _isAgentSelected = false;
  }

  if (_sim->step()) {
    updateSceneAgentCollection();
    updateAgentPosition(static_cast<int>(_sim->getNumAgents()));
    return true;
  }
  throw SystemStopException();
}

////////////////////////////////////////////////////////////////////////////

bool SimSystem::updateSceneAgentCollection() {
  size_t simAgentsCount = _sim->getNumAgents();

  if (simAgentsCount > _visAgents.size()) {
    // Clear the existing visAgents vector
    _visAgents.clear();

    _agentRoot->clearChildren();
    // Reserve memory for agents
    _visAgents.reserve(simAgentsCount);

    // Add all agents from the simulator
    for (size_t a = 0; a < simAgentsCount; ++a) {
      BaseAgent* agt = _sim->getAgent(a);
      VisAgent* baseNode = VisAgentDB::getInstance(agt);
      VisAgent* agtNode = baseNode->moveToClone();
      float h = _sim->getElevation(agt);
      agtNode->setPosition(agt->_pos.x(), agt->_pos.y(), h);
      _agentRoot->addChild(agtNode);
      _visAgents.push_back(agtNode);

      // update the selected agent with the updated reference to the right VisAgent
      if (_isAgentSelected && agt->_id == _selectedAgentId) {
        SceneGraph::Selectable::setSelectedObject(agtNode);
        _mengeContext->updateSelected();
      }
    }
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////

void SimSystem::addObstacleToScene() {
  // TODO: If the bsptree (ObstacleKDTree.h) chops up the obstacles, this isn't doing the
  //    right thing.  Currently, the bsptree chops them
  //  THIS IS A HACK to address the issues of the ObstacleKDTree
  //    The right thing to do is modify things so that they are not chopped up.
  std::set<const Obstacle*> handled;
  const std::vector<Obstacle*>& obstacles = _sim->getSpatialQuery()->getObstacles();
  for (size_t o = 0; o < obstacles.size(); ++o) {
    const Obstacle* obst = obstacles[o];
    if (handled.find(obst) == handled.end()) {
      Vector2 p0a = obst->getP0();
      Vector2 p1a = obst->getP1();
      const Obstacle* next = obst->_nextObstacle;
      while (next && next->_unitDir * obst->_unitDir >= 0.99999f) {
        handled.insert(next);
        p1a.set(next->getP1());
        next = next->_nextObstacle;
      }
      Vector3 p0(p0a.x(), p0a.y(), _sim->getElevation(p0a));
      Vector3 p1(p1a.x(), p1a.y(), _sim->getElevation(p1a));
      VisObstacle* vo = new VisObstacle(p0, p1);

      _obstacleRoot->addChild(vo);
      handled.insert(obst);
    }
  }
}

////////////////////////////////////////////////////////////////////////////

void SimSystem::addAgentsToScene() {
  size_t numAgents = _sim->getNumAgents();
  _visAgents.resize(numAgents);
  for (size_t a = 0; a < numAgents; ++a) {
    BaseAgent* agt = _sim->getAgent(a);
    VisAgent* baseNode = VisAgentDB::getInstance(agt);
    VisAgent* agtNode = baseNode->moveToClone();
    float h = _sim->getElevation(agt);
    agtNode->setPosition(agt->_pos.x(), agt->_pos.y(), h);
    _agentRoot->addChild(agtNode);
    _visAgents[a] = agtNode;
  }
}

////////////////////////////////////////////////////////////////////////////

void SimSystem::populateScene() {
  assert(_sim != 0x0 && "Can't add SimSystem to scene when no simulator is connected");

  _agentRoot = new MengeVis::SceneGraph::GLDagNode();
  _scene->addNode(_agentRoot);

  addAgentsToScene();

  _obstacleRoot = new MengeVis::SceneGraph::GLDagNode();
  _scene->addNode(_obstacleRoot);

  addObstacleToScene();
}

////////////////////////////////////////////////////////////////////////////

void SimSystem::updateAgentPosition(int agtCount) {
#pragma omp parallel for
  for (int a = 0; a < agtCount; ++a) {
    const BaseAgent* agt = _visAgents[a]->getAgent();
    float h = _sim->getElevation(agt);
    _visAgents[a]->setPosition(agt->_pos.x(), agt->_pos.y(), h);
  }
}

////////////////////////////////////////////////////////////////////////////

size_t SimSystem::getAgentCount() const { return _sim->getNumAgents(); }

////////////////////////////////////////////////////////////////////////////
}  // namespace Runtime
}  // namespace MengeVis
