#include "pch.h"
#include "StateMachineSystem.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "Component/StateMachineComponent.h"

StateMachineSystem::StateMachineSystem()
{
	m_hashCode = typeid(StateMachineComponent).hash_code();
}

void StateMachineSystem::Init(GameManager* _gameManager)
{
	m_componentManager = _gameManager->GComponentManager();
}

void StateMachineSystem::Update(GameManager* _gameManager)
{
    for (uint32 entityId : m_entities)
    {
        if (entityId == MAX_VALUE) continue;

		StateMachineComponent* smComp = m_componentManager->GComponent<StateMachineComponent>(entityId);

        if (smComp && smComp->machine)
        {
            smComp->machine->Update(entityId);
        }
    }
}