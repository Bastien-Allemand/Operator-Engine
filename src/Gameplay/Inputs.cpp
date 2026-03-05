#include "pch.h"
#include "Inputs.h"
#include "ProjectilePrefab.h"
#include "Component/ColliderComponent.h"
#include "Component/LightComponent.h"
#include "ShootComponent.h"
#include "TransitionComponent.h"
#include "StockProjectileComponent.h"
#include "HealthComponent.h"
#include "MoveComponent.h"
#include "TypeProjectileComponent.h"

#include "Cbuffers.h"
#include "Component/PostProcessComponent.h"

void MoveLeft(uint32 _entity, GameManager* _gameManager)
{
	TransformComponent* transform = _gameManager->GComponentManager()->GComponent<TransformComponent>(_entity);
	transform->localTransform.MoveRight(-1 * _gameManager->GCDeltaTime());
}

void MoveRight(uint32 _entity, GameManager* _gameManager)
{
	TransformComponent* transform = _gameManager->GComponentManager()->GComponent<TransformComponent>(_entity);
	transform->localTransform.MoveRight(1 * _gameManager->GCDeltaTime());
}

void MoveFoward(uint32 _entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(_entity);
	transform->localTransform.MoveFoward(1 * _gameManager->GCDeltaTime());
}

void MoveBackward(uint32 _entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(_entity);
	transform->localTransform.MoveFoward(-1 * _gameManager->GCDeltaTime());
}

void MoveUp(uint32 _entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(_entity);
	transform->localTransform.MoveUp(1 * _gameManager->GCDeltaTime());
}

void MoveDown(uint32 _entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(_entity);
	transform->localTransform.MoveUp(-1 * _gameManager->GCDeltaTime());
}

void TurnLeft(uint32 _entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(_entity);
	transform->localTransform.AddYPR(-1 * _gameManager->GCDeltaTime(), 0, 0);
}

void TurnRight(uint32 _entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(_entity);
	transform->localTransform.AddYPR(1 * _gameManager->GCDeltaTime(), 0, 0);
}

void TurnUp(uint32 _entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(_entity);
	transform->localTransform.AddYPR(0, -1 * _gameManager->GCDeltaTime(), 0);
}

void TurnDown(uint32 _entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(_entity);
	transform->localTransform.AddYPR(0, 1 * _gameManager->GCDeltaTime(), 0);
}

void Slow(uint32 _entity, GameManager* _gameManager)
{
	float speedTime = _gameManager->GCSpeedTime();
	if (speedTime == 0) return;
	_gameManager->SSpeedTime(0.1f);
}

void Boost(uint32 _entity, GameManager* _gameManager)
{
	float speedTime = _gameManager->GCSpeedTime();
	if (speedTime == 0) return;
	_gameManager->SSpeedTime(10.f);
}

void Normal(uint32 _entity, GameManager* _gameManager)
{
	float speedTime = _gameManager->GCSpeedTime();
	if (speedTime == 0) return;
	_gameManager->SSpeedTime(1.f);
}

void Shoot(uint32 _entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();

	ShootComponent* shootComp = componentManager->GComponent<ShootComponent>(_entity);

	if (shootComp->canShoot)
	{
		shootComp->canShoot = false;
		TransformComponent* transform = componentManager->GComponent<TransformComponent>(_entity);
		StockProjectileComponent* stock = componentManager->GComponent<StockProjectileComponent>(_entity);
		
		ProjectilePrefab newProjectile(stock->m_mesh,stock->m_texture, ProjectileType::PLAYER_PROJECTILE);
		newProjectile.transform->localTransform.position = transform->localTransform.position;
		newProjectile.transform->localTransform.forward = transform->localTransform.forward;
		newProjectile.transform->localTransform.MoveFoward(3.0f);
	}
}

void TogglePostProcess(uint32 _entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransitionComponent* transition = componentManager->GComponent<TransitionComponent>(_entity);
	ComponentPool<PostProcessComponent>* ppPool = componentManager->GPool<PostProcessComponent>();

	if (!ppPool || !transition) return;

	transition->Next();

	Vector<uint32> ppEntities = ppPool->GEntityIds();
	for (uint32 entityId : ppEntities)
	{
		PostProcessComponent* pp = ppPool->GComponent(entityId);
		if (!pp || !pp->shaderPath) continue;

		pp->isActive = true;

		bool isCel = (*pp->shaderPath == L"../../res/Shaders/Cel-shading.hlsl");
		bool isGame = (*pp->shaderPath == L"../../res/Shaders/GameShader.hlsl");
		bool isBlur = (*pp->shaderPath == L"../../res/Shaders/Blur.hlsl");

		switch (transition->state)
		{
		case State::VERYLOW:
			//                   levels  threshold edgeR   edgeG     edgeB  multiplier blend
			if (isCel) { pp->params0 = { 3.0f, 0.1f, 0.0f, 0.0f }; pp->params1 = { 0.0f, 2.5f, 1.0f, 0.0f }; }
			//                   sat   lum   contrast
			if (isGame) { pp->params0 = { 0.3f, 0.6f, 0.5f, 0.0f }; }
			//                   radius      isVertical
			if (isBlur) { pp->params0 = { 5.0f, 1.0f, 0.0f, 0.0f }; }
			break;
		case State::LOW: // Attenue, leger flou
			if (isCel) { pp->params0 = { 4.0f, 0.15f, 0.0f, 0.0f }; pp->params1 = { 0.0f, 1.5f, 0.9f, 0.0f }; }
			if (isGame) { pp->params0 = { 0.7f, 0.8f, 0.8f, 0.0f }; }
			if (isBlur) { pp->params0 = { 3.0f, 1.0f, 0.0f, 0.0f }; }
			break;
		case State::NORMAL: // Equilibre, pas de blur
			if (isCel) { pp->params0 = { 4.0f, 0.15f, 0.0f, 0.0f }; pp->params1 = { 0.0f, 1.5f, 1.0f, 0.0f }; }
			if (isGame) { pp->params0 = { 1.2f, 1.0f, 1.3f, 0.0f }; }
			if (isBlur) { pp->params0 = { 0.0f, 1.0f, 0.0f, 0.0f }; }
			break;
		case State::BRIGHT: // Vif, net, contours fins
			if (isCel) { pp->params0 = { 6.0f, 0.2f, 0.0f, 0.0f }; pp->params1 = { 0.0f, 1.0f, 0.8f, 0.0f }; }
			if (isGame) { pp->params0 = { 1.5f, 1.0f, 1.5f, 0.0f }; }
			if (isBlur) { pp->params0 = { 0.0f, 1.0f, 0.0f, 0.0f }; }
			break;
		default:
			break;
		}
	}
}

void Pause(uint32 _entity, GameManager* _gameManager)
{
	MoveComponent* moveComp = _gameManager->GComponentManager()->GComponent<MoveComponent>(_entity);
	HealthComponent* health = _gameManager->GComponentManager()->GComponent<HealthComponent>(_entity);
	if (health->health <=0) return;
	if (_gameManager->GCSpeedTime() != 0)
	{
		moveComp->isActive = false;
		moveComp->lockCursor = false;
		_gameManager->SSpeedTime(0);
	}
	else
	{
		moveComp->isActive = true;
		moveComp->lockCursor = true;
		_gameManager->SSpeedTime(1);
	}
}