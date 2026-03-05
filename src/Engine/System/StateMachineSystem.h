#pragma once
#include "ISystem.h"
class ComponentManager;

class StateMachineSystem : public ISystem
{
public:
	StateMachineSystem();
    void Init(GameManager* _gameManager) override;
    void Update(GameManager* _gameManager) override;
private:
	ComponentManager* m_componentManager;
};