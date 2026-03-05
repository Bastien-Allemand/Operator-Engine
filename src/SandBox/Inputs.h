#pragma once
class GameManager;	

void MoveLeft(uint32 entity, GameManager* _gameManager);
void MoveRight(uint32 entity, GameManager* _gameManager);
void MoveFoward(uint32 entity, GameManager* _gameManager);
void MoveBackward(uint32 entity, GameManager* _gameManager);
void MoveUp(uint32 entity, GameManager* _gameManager);
void MoveDown(uint32 entity, GameManager* _gameManager);
void TurnLeft(uint32 entity, GameManager* _gameManager);
void TurnRight(uint32 entity, GameManager* _gameManager);
void TurnUp(uint32 entity, GameManager* _gameManager);
void TurnDown(uint32 entity, GameManager* _gameManager);
void SlowDown(uint32 entity, GameManager* _gameManager);
void Mouselock(uint32 entity, GameManager* _gameManager);
void SpeedUp(uint32 entity, GameManager* _gameManager);
void SpawnProjectil(uint32 entity, GameManager* _gameManager);