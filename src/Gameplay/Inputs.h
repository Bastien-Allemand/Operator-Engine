#pragma once
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "Component/TransformComponent.h"

void MoveLeft(uint32 _entity, GameManager* _gameManager);

void MoveRight(uint32 _entity, GameManager* _gameManager);

void MoveFoward(uint32 _entity, GameManager* _gameManager);


void MoveBackward(uint32 _entity, GameManager* _gameManager);


void MoveUp(uint32 _entity, GameManager* _gameManager);


void MoveDown(uint32 _entity, GameManager* _gameManager);


void TurnLeft(uint32 _entity, GameManager* _gameManager);


void TurnRight(uint32 _entity, GameManager* _gameManager);


void TurnUp(uint32 _entity, GameManager* _gameManager);


void TurnDown(uint32 _entity, GameManager* _gameManager);


void Slow(uint32 _entity, GameManager* _gameManager);

void Boost(uint32 _entity, GameManager* _gameManager);

void Normal(uint32 _entity, GameManager* _gameManager);

void Shoot(uint32 _entity, GameManager* _gameManager);

void TogglePostProcess(uint32 _entity, GameManager* _gameManager);

void Pause(uint32 _entity, GameManager* _gameManager);
