#include "pch.h"
#include "InputSystem.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "Manager/InputManager.h"
#include "ComponentPool.h"
#include "Component/InputComponent.h"

InputSystem::InputSystem()
{
	m_hashCode = typeid(IInputComponent).hash_code();

}

void InputSystem::Init(GameManager* _gameManager)
{
	m_inputManager = InputManager::GInstance();
	m_componentManager = _gameManager->GComponentManager();
}

void InputSystem::Update(GameManager* _gameManager)
{

	for (uint32 entity : m_entities)
	{
		IInputComponent* component = m_componentManager->GComponent<IInputComponent>(entity);
		if (component == nullptr) continue;	
		for (Condition condition : component->conditions )
		{
			switch (condition.actionType)
			{
			case Condition::DOWN:
				if (m_inputManager->IsKeyDown(condition.key))
					component->actions[condition.actionIndex](entity, _gameManager);
				break;
			case Condition::PRESSED:
				if (m_inputManager->IsKeyPressed(condition.key))
					component->actions[condition.actionIndex](entity,_gameManager);
				break;
			case Condition::RELEASED:
				if (m_inputManager->IsKeyReleased(condition.key))
					component->actions[condition.actionIndex](entity, _gameManager);
				break;
			case Condition::UP:
				if (m_inputManager->IskeyUp(condition.key))
					component->actions[condition.actionIndex](entity, _gameManager);
				break;
			}
		}
	}
}
