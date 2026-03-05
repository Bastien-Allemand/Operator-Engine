#include "pch.h"
#include "ComponentManager.h"

ComponentManager::ComponentManager(SystemManager* _systemManager)
{
	m_systemManager = _systemManager;
	m_componentPools.reserve(4096);
}

void ComponentManager::RemoveEntity(uint32 _entityId)
{
	for (IComponentPool* pool : m_componentPools)
	{
		pool->Remove(_entityId);
	}
	m_systemManager->RemoveEntity(_entityId);
}
