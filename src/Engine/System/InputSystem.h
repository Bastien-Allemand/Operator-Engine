#pragma once
#include "ISystem.h"
class GameManager;
class InputManager;
class ComponentManager;

class InputSystem : public ISystem
{
public:
	InputSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager)override;
private:
	InputManager* m_inputManager;
	ComponentManager* m_componentManager;
};

