#pragma once
#include "ISystem.h"

class GameManager;
class ComponentManager;

class MovementSystem : public ISystem
{
public: 
	MovementSystem();
	void Init(GameManager* _gameManager) override;	
	void Update(GameManager* _gameManager) override;
private:
	ComponentManager* m_componentManager;

	float m_sensitivity = 0.01f;
	Vector2i m_previousMousePos = {-1,-1};

	bool visibleMouse = true;
};

