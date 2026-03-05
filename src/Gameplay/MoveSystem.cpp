#include "pch.h"
#include "MoveSystem.h"
#include "MoveComponent.h"
#include "Component/TransformComponent.h"
#include "Component/ColliderComponent.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"

#include "Core/Window.h"

MoveSystem::MoveSystem()
{
	m_hashCode = typeid(MoveComponent).hash_code();
}

void MoveSystem::Init(GameManager* _gameManager)
{
	m_componentManager = _gameManager->GComponentManager();
}

void MoveSystem::Update(GameManager* _gameManager)
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
		MoveComponent* movement = m_componentManager->GComponent<MoveComponent>(entityId);
		if (!movement->isActive)
			continue;
		Vector2f mouseDelta = { (float)currentMousePos.x - (float)m_previousMousePos.x,  (float)currentMousePos.y - (float)m_previousMousePos.y };
		TransformComponent* transform = m_componentManager->GComponent<TransformComponent>(entityId);
		transform->localTransform.position.y = 0.1f;
		if (transform == nullptr)
			continue;
		float pitch = std::clamp(mouseDelta.y, -89.0f, 89.0f);
		movement->yaw += mouseDelta.x * m_sensitivity;
		movement->pitch += pitch * m_sensitivity;
		transform->localTransform.SYPR(movement->yaw, movement->pitch, 0);
		//transform->localTransform.AddYPR(mouseDelta.x * m_sensitivity, pitch * m_sensitivity, 0);

		m_previousMousePos = currentMousePos;

		if (movement->lockCursor)
		{
			_gameManager->SMousePosition(Vector2i(_gameManager->GWindow()->GCWidth() / 2, _gameManager->GWindow()->GCHeight() / 2));
			m_previousMousePos = _gameManager->GMousePosition();
		}
	}
}