#pragma once
#include "ISystem.h"

class GameManager;
class ComponentManager;
class IRenderer;

class CameraSystem : public ISystem
{
public:
	CameraSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
private:
	ComponentManager* m_componentManager;
	IRenderer* m_renderer;
};

