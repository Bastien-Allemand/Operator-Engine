#include "pch.h"
#include "BulletMovementSystem.h"
#include "BulletComponent.h"
#include "Component/TransformComponent.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"

BulletMovementSystem::BulletMovementSystem()
{
	m_hashCode = typeid(BulletComponent).hash_code();
}

void BulletMovementSystem::Init(GameManager* _gameManager)
{
}

void BulletMovementSystem::Update(GameManager* _gameManager)
{
	float dt = _gameManager->GCDeltaTime();
	ComponentManager* componentManager = _gameManager->GComponentManager();
	for (uint32 entityId : m_entities)
	{
		BulletComponent* bulletComonent = componentManager->GComponent<BulletComponent>(entityId);
		if (bulletComonent->lifedelay< bulletComonent->lifetime )
		{
			TransformComponent* transform = componentManager->GComponent<TransformComponent>(entityId);

			transform->localTransform.MoveFoward(bulletComonent->speed * dt);
			bulletComonent->lifedelay += dt;
		}
		else
		{
			_gameManager->DestroyEntity(entityId);
			//destroy the bullet
		}
	}
}
