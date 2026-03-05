#pragma once
#include "ISystem.h" 

class GameManager;

class TransformSystem : public ISystem
{
public:
	TransformSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
};

