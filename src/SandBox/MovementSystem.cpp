#include "pch.h"
#include "MovementSystem.h"
#include "MovementComponent.h"
#include "Component/TransformComponent.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"

#include "Core/Window.h"

MovementSystem::MovementSystem()
{
	m_hashCode = typeid(MovementComponent).hash_code();
}

void MovementSystem::Init(GameManager* _gameManager)
{
	m_componentManager = _gameManager->GComponentManager();
}

void MovementSystem::Update(GameManager* _gameManager)
{
	Vector2i currentMousePos = _gameManager->GMousePosition();
	if (!_gameManager->GWindow()->IsFocused())
	{
		m_previousMousePos = currentMousePos;
		if (visibleMouse)
		{
			visibleMouse = false;
			_gameManager->GWindow()->HideCursor(visibleMouse);
		}
		return;
	}
	if (!visibleMouse)
	{
		visibleMouse = true;
		_gameManager->GWindow()->HideCursor(visibleMouse);
	}
	for (uint32 entityId : m_entities)
	{
		MovementComponent* movement = m_componentManager->GComponent<MovementComponent>(entityId);
		if (!movement->isActive)
			continue;
		Vector2f mouseDelta = {(float)currentMousePos.x - (float)m_previousMousePos.x,  (float)currentMousePos.y - (float)m_previousMousePos.y};
		TransformComponent* transform = m_componentManager->GComponent<TransformComponent>(entityId);
		if (transform == nullptr)
			continue;
		float pitch = std::clamp(mouseDelta.y, -89.0f, 89.0f);
		transform->localTransform.AddYPR(mouseDelta.x * m_sensitivity, pitch * m_sensitivity, 0);

		m_previousMousePos = currentMousePos;

		if (movement->lockCursor)
		{
			_gameManager->SMousePosition(Vector2i(_gameManager->GWindow()->GCWidth() / 2, _gameManager->GWindow()->GCHeight() / 2));
			m_previousMousePos = _gameManager->GMousePosition();
		}
	}
}
