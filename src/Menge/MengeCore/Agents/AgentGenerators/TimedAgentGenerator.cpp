#include "TimedAgentGenerator.h"

#include "MengeCore/Agents/BaseAgent.h"
#include "MengeCore/Math/consts.h"
#include "MengeCore/Runtime/Logger.h"
#include "thirdParty/tinyxml.h"

namespace Menge {

namespace Agents {

TimedAgentGenerator::TimedAgentGenerator() : RectGridGenerator(), _spawnSchedule() {
  _nextSpawn = _spawnSchedule.begin();
}

void TimedAgentGenerator::setSpawnSchedule(const std::map<float, size_t>& schedule) {
  _spawnSchedule = schedule;
  _nextSpawn = _spawnSchedule.begin();
}

bool TimedAgentGenerator::shouldSpawn(float time, size_t& count) {
  if (_nextSpawn != _spawnSchedule.end() && time >= _nextSpawn->first) {
    count = _nextSpawn->second;
    ++_nextSpawn;
    return true;
  }
  return false;
}

void TimedAgentGenerator::setAgentPosition(size_t i, BaseAgent* agt) {}

bool TimedAgentGenerator::setAgentProfile(ProfileSelector*) const { return false; }

ProfileSelector* TimedAgentGenerator::getAgentProfile() const { return nullptr; }

TimedAgentGeneratorFactory::TimedAgentGeneratorFactory() : RectGridGeneratorFactory() {
  _spawnScheduleID = _attrSet.addStringAttribute("spawn_schedule", true, "");
}

bool TimedAgentGeneratorFactory::setFromXML(AgentGenerator* gen, TiXmlElement* node,
                                            const std::string& specFldr) const {
  TimedAgentGenerator* timedGen = dynamic_cast<TimedAgentGenerator*>(gen);
  assert(timedGen != 0x0 &&
         "Trying to set attributes of a timed agent generator component on an incompatible object");

  if (!RectGridGeneratorFactory::setFromXML(timedGen, node, specFldr)) return false;

  std::string scheduleStr = _attrSet.getString(_spawnScheduleID);
  std::istringstream iss(scheduleStr);
  std::map<float, size_t> schedule;
  float time;
  size_t count;

  while (iss >> time >> count) {
    schedule[time] = count;
  }

  if (iss.fail() && !iss.eof()) {
    logger << Logger::ERR_MSG << "Error parsing spawn schedule in timed agent generator.";
    return false;
  }

  timedGen->setSpawnSchedule(schedule);

  return true;
}

}  // namespace Agents

}  // namespace Menge
