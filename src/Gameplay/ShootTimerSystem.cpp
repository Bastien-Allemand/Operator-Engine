#include "pch.h"
#include "ShootTimerSystem.h"
#include "ShootComponent.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"

ShootTimerSystem::ShootTimerSystem()
{
	m_hashCode = typeid(ShootComponent).hash_code();
}

void ShootTimerSystem::Init(GameManager* _gameManager)
{

}

void ShootTimerSystem::Update(GameManager* _gameManager)
{
	for (uint16 entityId : m_entities)
	{
		ShootComponent* shootComp = _gameManager->GComponentManager()->GComponent<ShootComponent>(entityId);
		if (shootComp == nullptr) continue;
		if (!shootComp->canShoot)
		{
			shootComp->fireTimer += _gameManager->GCDeltaTime();
			if (shootComp->fireTimer >= shootComp->fireRate)
			{
				shootComp->canShoot = true;
				shootComp->fireTimer = 0.f;
			}
		}
	}
}
