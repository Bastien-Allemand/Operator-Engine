#pragma once
#include "ISystem.h"

class GameManager;
class ComponentManager;
class IRenderer;

class LightSystem : public ISystem
{
public:
	LightSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
private:
	ComponentManager* m_componentManager;
	IRenderer* m_renderer;
};

