#pragma once
#include "ISystem.h"
class ShootTimerSystem : public ISystem
{
	public:
	ShootTimerSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
};

