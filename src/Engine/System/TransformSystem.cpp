#include "pch.h"
#include "TransformSystem.h"
#include "Manager/GameManager.h"
#include "Component/TransformComponent.h"
#include "Manager/ComponentManager.h"
#include "ComponentPool.h"

TransformSystem::TransformSystem()
{
	m_hashCode = typeid(TransformComponent).hash_code();
}

void TransformSystem::Init(GameManager* _gameManager)
{

}

void TransformSystem::Update(GameManager* _gameManager)
{
	for (uint32 entity : m_entities)
	{
		if (entity == -1) continue;


	}
}