#include "ConstantPersistentAgentGenerator.h"

size_t Menge::Agents::ConstantPersistentAgentGenerator::agentCount() { return size_t(); }

void Menge::Agents::ConstantPersistentAgentGenerator::setAgentPosition(size_t i, BaseAgent* agt) {}

bool Menge::Agents::ConstantPersistentAgentGenerator::shouldGenerate() const { return false; }
