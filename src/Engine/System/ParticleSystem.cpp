#include "pch.h"
#include "ParticleSystem.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "ComponentPool.h"
#include "Component/ParticleComponent.h"
#include "Component/TransformComponent.h"
#include "Component/ParticleEmitterComponent.h"
#include "IRenderer.h"

ParticleSystem::ParticleSystem()
{
	m_hashCode = typeid(ParticleComponent).hash_code();
}
void ParticleSystem::Init(GameManager* _gameManager)
{
	m_renderer = _gameManager->GRenderEngine();
	m_componentManager = _gameManager->GComponentManager();
}

void ParticleSystem::Update(GameManager* _gameManager)
{
	ComponentPool<ParticleComponent>* particlePool = m_componentManager->GPool<ParticleComponent>();
	ComponentPool<TransformComponent>* transformPool = m_componentManager->GPool<TransformComponent>();
	ComponentPool<ParticleEmitterComponent>* emitterPool = m_componentManager->GPool<ParticleEmitterComponent>();

	for (uint32 entityId : m_entities)
	{
		ParticleComponent* pc = particlePool->GComponent(entityId);
		if (!pc || !pc->isInitialized || !pc->gpuResource) continue;

		TransformComponent* tc = transformPool->GComponent(entityId);
		Vector3f emitPos = tc ? tc->localTransform.position : Vector3f(0, 0, 0);

		ParticleEmitterComponent* emitter = emitterPool->GComponent(entityId);
		emitter->m_texture = m_renderer->LoadTexture(emitter->m_pathTexture->c_str());

		ParticleUpdateParams params;
		params.emitterPosition = emitPos;
		params.dt = _gameManager->GCDeltaTime();
		emitter->accumulator += _gameManager->GCDeltaTime();
		float interval = 1.0f / emitter->spawnRate;
		params.numToSpawn = static_cast<uint32_t>(emitter->accumulator / interval);
		emitter->accumulator -= params.numToSpawn * interval;
		params.particleType = emitter->particleType;
		params.randomSeed = rand();
		params.minVelocity = emitter->minVelocity;
		params.minLifetime = emitter->minLifetime;
		params.maxVelocity = emitter->maxVelocity;
		params.maxLifetime = emitter->maxLifetime;
		params.startColor = emitter->startColor;
		params.endColor = emitter->endColor;
		params.startSize = emitter->startSize;
		params.endSize = emitter->endSize;
		params.shapeType = static_cast<int>(emitter->shape);
		params.shapeRadius = emitter->shapeRadius;
		params.shapeBoxExtents = emitter->shapeBoxExtents;
		params.direction = emitter->direction;
		params.force = emitter->force;

		m_renderer->UpdateParticles(pc->gpuResource, params);
		m_renderer->DrawParticles(pc->gpuResource, emitter->m_texture);
	}
}
