#ifndef __TIMED_AGENT_GENERATOR_H__
#define __TIMED_AGENT_GENERATOR_H__

#include <map>
#include <vector>

#include "MengeCore/Agents/AgentGenerators/RectGridGenerator.h"
#include "MengeCore/Agents/AgentGenerators/RuntimeAgentGenerator.h"
#include "MengeCore/Math/consts.h"
#include "MengeCore/Runtime/Logger.h"

namespace Menge {

namespace Agents {

class TimedAgentGenerator : public RectGridGenerator, public RuntimeAgentGenerator{
 public:
  TimedAgentGenerator();

  void setSpawnSchedule(const std::map<float, size_t>& schedule);
  bool shouldSpawn(float time, size_t& count);

 private:
  std::map<float, size_t> _spawnSchedule;
  std::map<float, size_t>::iterator _nextSpawn;

  // Inherited via RuntimeAgentGenerator
  virtual void setAgentPosition(size_t i, BaseAgent* agt) override;
  virtual bool setAgentProfile(ProfileSelector*) const override;
  virtual ProfileSelector* getAgentProfile() const override;
};

class TimedAgentGeneratorFactory : public RectGridGeneratorFactory {
 public:
  TimedAgentGeneratorFactory();

  virtual const char* name() const { return "timed_agent_generator"; }
  virtual const char* description() const {
    return "Generates agents at specific times during the simulation in a rectangular grid";
  }

 protected:
  AgentGenerator* instance() const { return new TimedAgentGenerator(); }
  bool setFromXML(AgentGenerator* gen, TiXmlElement* node, const std::string& specFldr) const;

  size_t _spawnScheduleID;
};

}  // namespace Agents

}  // namespace Menge

#endif  // __TIMED_AGENT_GENERATOR_H__
