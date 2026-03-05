#pragma once
#include "ISystem.h"

struct ScoreComponent;

class HealthSystem : public ISystem
{
	public:
	HealthSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
private:
	int m_entityPlayerID = -1;
	bool m_updateText = false;	
	bool m_playerDead = false;
};

