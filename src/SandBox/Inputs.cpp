#include "pch.h"
#include "Inputs.h"
#include "Component/InputComponent.h"
#include "Component/TransformComponent.h"
#include "MovementComponent.h"
#include "Component/MeshComponent.h"
#include "ProjectilComponent.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"

void MoveLeft(uint32 entity, GameManager* _gameManager)
{
	TransformComponent* transform = _gameManager->GComponentManager()->GComponent<TransformComponent>(entity);
	transform->localTransform.MoveRight(-1 * _gameManager->GCDeltaTime());
}

void MoveRight(uint32 entity, GameManager* _gameManager)
{
	TransformComponent* transform = _gameManager->GComponentManager()->GComponent<TransformComponent>(entity);
	transform->localTransform.MoveRight(1 * _gameManager->GCDeltaTime());
}

void MoveFoward(uint32 entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(entity);
	transform->localTransform.MoveFoward(1 * _gameManager->GCDeltaTime());
}

void MoveBackward(uint32 entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(entity);
	transform->localTransform.MoveFoward(-1 * _gameManager->GCDeltaTime());
}

void MoveUp(uint32 entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(entity);
	transform->localTransform.MoveUp(1 * _gameManager->GCDeltaTime());
}

void MoveDown(uint32 entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(entity);
	transform->localTransform.MoveUp(-1 * _gameManager->GCDeltaTime());
}

void TurnLeft(uint32 entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(entity);
	transform->localTransform.AddYPR(-1 * _gameManager->GCDeltaTime(), 0, 0);
}

void TurnRight(uint32 entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(entity);
	transform->localTransform.AddYPR(1 * _gameManager->GCDeltaTime(), 0, 0);
}

void TurnUp(uint32 entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(entity);
	transform->localTransform.AddYPR(0, -1 * _gameManager->GCDeltaTime(), 0);
}

void TurnDown(uint32 entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(entity);
	transform->localTransform.AddYPR(0, 1 * _gameManager->GCDeltaTime(), 0);
}

void SlowDown(uint32 entity, GameManager* _gameManager)
{
	float speedTime = _gameManager->GCSpeedTime() - 0.1f;
	if (speedTime < 0)
	{
		speedTime = 0;
	}
	_gameManager->SSpeedTime(speedTime);
}

void SpeedUp(uint32 entity, GameManager* _gameManager)
{
	float speedTime = _gameManager->GCSpeedTime() + 0.1f;
	_gameManager->SSpeedTime(speedTime);
}

void Mouselock(uint32 entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	MovementComponent* movement = componentManager->GComponent<MovementComponent>(entity);
	movement->lockCursor = !movement->lockCursor;
}

void SpawnProjectil(uint32 entity, GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	TransformComponent* transform = componentManager->GComponent<TransformComponent>(entity);

	uint32 id = _gameManager->GNewEntityId();

	TransformComponent* projectilTransform = componentManager->AddComponent<TransformComponent>(id);

	ProjectilComponent* projectilComponent = componentManager->AddComponent<ProjectilComponent>(id);
	projectilComponent->toSpawn = true;
}