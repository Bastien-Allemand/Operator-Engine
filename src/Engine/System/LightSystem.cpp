#include "pch.h"
#include "LightSystem.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"

#include "IRenderer.h"

#include "Component/LightComponent.h"
#include "Component/TransformComponent.h"

LightSystem::LightSystem()
{
	m_hashCode = typeid(LightComponent).hash_code();

}

void LightSystem::Init(GameManager* _gameManager)
{
	m_renderer = _gameManager->GRenderEngine();
	m_componentManager = _gameManager->GComponentManager();
}

void LightSystem::Update(GameManager* _gameManager)
{
	for (uint32& entityId : m_entities)
	{
		LightComponent* lightComponent = m_componentManager->GComponent<LightComponent>(entityId);
		if (!lightComponent->initialize)
		{
			m_renderer->SLight(lightComponent->LightData,lightComponent->Type);
			lightComponent->initialize = true;
		}
		TransformComponent* transformComponent = m_componentManager->GComponent<TransformComponent>(entityId);

	}
}
