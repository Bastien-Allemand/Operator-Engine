#include "pch.h"
#include "ISystem.h"

void ISystem::AddEntity(uint32 _entityId, size_t _hashCode)
{
	if (m_hashCode == _hashCode)
	{
		auto it = std::find(m_entities.begin(), m_entities.end(), _entityId);
		if (it == m_entities.end())
		{
			m_entities.push_back(_entityId);
		}
	}
}
void ISystem::RemoveEntity(uint32 _entityId)
{
	auto it = std::find(m_entities.begin(), m_entities.end(), _entityId);
	if (it != m_entities.end())
	{
		m_entities.erase(it);
	}
}
void ISystem::RemoveComponent(uint32 _entityId, size_t _hashCode)
{
	if (m_hashCode == _hashCode)
	{
		auto it = std::find(m_entities.begin(), m_entities.end(), _entityId);
		if (it != m_entities.end())
		{
			m_entities.erase(it);
		}
	}
}