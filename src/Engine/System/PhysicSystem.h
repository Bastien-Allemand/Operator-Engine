#pragma once
#include "ISystem.h"

class ComponentManager;
class GameManager;

class PhysicSystem : public ISystem
{
	public:
	PhysicSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
private:
	ComponentManager* m_componentManager;
};