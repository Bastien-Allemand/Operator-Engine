#include "pch.h"
#include "CameraSystem.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "Render/RenderEngine.h"
#include "Component/TransformComponent.h"
#include "Component/CameraComponent.h"
#include "ComponentPool.h"

CameraSystem::CameraSystem()
{
	m_hashCode = typeid(CameraComponent).hash_code();
}

void CameraSystem::Init(GameManager* _gameManager)
{
	m_componentManager = _gameManager->GComponentManager();
	m_renderer = _gameManager->GRenderEngine();
}

void CameraSystem::Update(GameManager* _gameManager)
{
	bool mainCameraFound = false;

	for (uint32 entityId : m_entities)
	{
		if (entityId == -1) continue;
		CameraComponent* cameraComponent = m_componentManager->GComponent<CameraComponent>(entityId);
		TransformComponent* transform = m_componentManager->GComponent<TransformComponent>(entityId);
		if (transform != nullptr)
		{
			if (cameraComponent->swapTothisCamera)
			{
				mainCameraFound = true;
				cameraComponent->swapTothisCamera = false;
				m_renderer->SCameraTransform(&transform->localTransform);
				m_renderer->SCamera(cameraComponent->camera);
				cameraComponent->mainCamera = true;
			}
			else if (cameraComponent->mainCamera && mainCameraFound)
			{
				cameraComponent->mainCamera = false;
			}
			else if (cameraComponent->mainCamera && !mainCameraFound)
			{
				mainCameraFound = true;
			}
		}
	}
}
