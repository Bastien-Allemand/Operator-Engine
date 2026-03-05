#pragma once
#include "ISystem.h"

class IRenderer;
template<typename T>
class ComponentPool;
struct UITransformComponent;

class ComponentManager;
class GameManager;

class UISystem : public ISystem
{
public:
	UISystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
	void ComputeScreenRect(GameManager* _gameManager);

private:
	IRenderer* m_renderer;
	ComponentManager* m_componentManager;

	ComponentPool<UITransformComponent>* m_uiPool = nullptr;
};