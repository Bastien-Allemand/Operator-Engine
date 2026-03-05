#pragma once
#include "ISystem.h"

class GameManager;
class IRenderer;
class ComponentManager;

class PostProcessSystem : public ISystem
{
	public:
	PostProcessSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
private:
	ComponentManager* m_componentManager;
	IRenderer* m_renderEngine;
};

