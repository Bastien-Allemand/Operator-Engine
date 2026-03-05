#include "pch.h"
#include "PostProcessSystem.h"

#include "Component/PostProcessComponent.h"

#include "Manager/GameManager.h"
#include "IRenderer.h"
#include "Manager/ComponentManager.h"

PostProcessSystem::PostProcessSystem()
{
	m_hashCode = typeid(PostProcessComponent).hash_code();
}

void PostProcessSystem::Init(GameManager* _gameManager)
{
	m_componentManager = _gameManager->GComponentManager();
	m_renderEngine = _gameManager->GRenderEngine();
}

void PostProcessSystem::Update(GameManager* _gameManager)
{
	ComponentPool<PostProcessComponent>* postProcessPool = m_componentManager->GPool<PostProcessComponent>();

	float time = _gameManager->GTimer();
	float deltaTime = _gameManager->GCDeltaTime();

	std::stable_sort(m_entities.begin(), m_entities.end(), [&](uint32 a, uint32 b) {
		PostProcessComponent* cA = postProcessPool->GComponent(a);
		PostProcessComponent* cB = postProcessPool->GComponent(b);
		if (cA == nullptr || cB == nullptr) return false;
		return cA->zOrder < cB->zOrder;
		});

	for (uint32 entityId : m_entities)
	{
		PostProcessComponent* postProcessComponent = postProcessPool->GComponent(entityId);
		if (postProcessComponent == nullptr)	continue;
		if (!postProcessComponent->isActive) continue;
		PostProcessParams params;
		params.time = time;
		params.deltaTime = deltaTime;
		params.params0 = postProcessComponent->params0;
		params.params1 = postProcessComponent->params1;
		m_renderEngine->DrawPostProcess(postProcessComponent->postProcessResource, params, *postProcessComponent->shaderPath);
	}
}
