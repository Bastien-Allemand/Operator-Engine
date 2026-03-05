#pragma once

#include "ComponentPool.h"
#include "SystemManager.h"

class ComponentManager
{
public:
	ComponentManager(SystemManager* _systemManager);

	template<typename T>
	ComponentPool<T>* GPool();

	template<typename T>
	T* AddComponent(uint32 _entityId);

	template<typename T>
	T* GComponent(uint32 _entityId);



	template<typename T>
	void RemoveComponent(uint32 _entityId);
	/// <summary>
	/// removes all the components of the entity from all the pools
	/// </summary>
	/// <param name="_entityId"></param>
	void RemoveEntity(uint32 _entityId);

private:
	SystemManager* m_systemManager;
	Vector<IComponentPool*> m_componentPools;
	Vector<size_t> m_poolId;
};

template<typename T>
inline ComponentPool<T>* ComponentManager::GPool()
{
	size_t typeId = typeid(T).hash_code();
	size_t index = -1;
	for (size_t i = 0; i < m_poolId.size(); i++)
	{
		if (typeId == m_poolId[i])
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		ComponentPool<T>* newPool = new ComponentPool<T>();

		m_componentPools.push_back(newPool);
		m_poolId.push_back(typeId);

		return static_cast<ComponentPool<T>*>(m_componentPools.back());
	}
	else
	{
		return static_cast<ComponentPool<T>*>(m_componentPools[index]);
	}
}

template<typename T>
inline T* ComponentManager::AddComponent(uint32 _entityId)
{
	ComponentPool<T>* pool = GPool<T>();
	m_systemManager->AddEntity(_entityId, typeid(T).hash_code());
	return pool->AddComponent(_entityId);
}

template<typename T>
inline T* ComponentManager::GComponent(uint32 _entityId)
{
	ComponentPool<T>* pool = GPool<T>();
	return pool->GComponent(_entityId);
}

template<typename T>
inline void ComponentManager::RemoveComponent(uint32 _entityId)
{
	ComponentPool<T>* pool = GPool<T>();
	pool->Remove(_entityId);
}
