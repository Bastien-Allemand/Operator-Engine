#pragma once
#include "ISystem.h"
class BulletMovementSystem : public ISystem
{
public:
	BulletMovementSystem();
	void Init(GameManager* _gameManager) override;

	void Update(GameManager* _gameManager) override;
};

