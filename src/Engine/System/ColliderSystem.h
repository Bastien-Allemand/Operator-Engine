#pragma once
#include "ISystem.h"

class GameManager;
class ComponentManager;


class ColliderSystem : public ISystem
{
public:
	ColliderSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
private:
	ComponentManager* m_componentManager;
};