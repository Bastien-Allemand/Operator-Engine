#include "pch.h"
#include "main.h"
#include "App.h"
#include "Manager/GameManager.h"

//class RightState : public IState
//{
//public:
//	void OnEnter(uint32 _entityId) override
//	{
//		std::cout << "Entered Right State" << std::endl;
//	}
//
//	void OnUpdate(uint32 _entityId) override
//	{
//	}
//
//	void OnExit(uint32 _entityId) override
//	{
//		std::cout << "Exited Right State" << std::endl;
//	}
//};
//
//class LeftState : public IState
//{
//public:
//	void OnEnter(uint32 _entityId) override
//	{
//		std::cout << "Entered Left State" << std::endl;
//	}
//
//	void OnUpdate(uint32 _entityId) override
//	{
//	}
//
//	void OnExit(uint32 _entityId) override
//	{
//		std::cout << "Exited Left State" << std::endl;
//	}
//};
//
//class IdleState : public IState
//{
//public:
//	void OnEnter(uint32 _entityId) override
//	{
//		std::cout << "Entered Idle State" << std::endl;
//	}
//
//	void OnUpdate(uint32 _entityId) override
//	{
//	}
//
//	void OnExit(uint32 _entityId) override
//	{
//		std::cout << "Exited Idle State" << std::endl;
//	}
//};
//
//enum class PlayerState
//{
//	idle = 0,
//	left,
//	right,
//	count
//};
//
//static constexpr int PlayerStateCount = static_cast<int>(PlayerState::count);
//
//// Tableau de transition : [EtatActuel][NouvelEtat]
//// 1 = Transition autoris�e, 0 = Transition bloqu�e
//int PlayerTransitions[PlayerStateCount][PlayerStateCount] =
//{
//	//I, L, R  
//	{ 0, 1, 1 }, // i 
//	{ 1, 0, 1 }, // l
//	{ 1, 1, 0 }  // r
//};
//
//PlayerState GetPlayerStateEnum(const std::string& stateName)
//{
//	if (stateName == "LEFT") return PlayerState::left;
//	if (stateName == "RIGHT") return PlayerState::right;
//	return PlayerState::idle;
//}
//
//bool ChangePlayerState(uint32 entity, PlayerState targetState, const std::string& stateName)
//{
//	GameManager* gameManager = GameManager::GInstance();
//	ComponentManager* componentManager = gameManager->GComponentManager();
//	StateMachineComponent* sm = componentManager->GComponent<StateMachineComponent>(entity);
//
//	if (!sm || !sm->machine)
//	{
//		return true;
//	}
//
//	int currentIndex = static_cast<int>(GetPlayerStateEnum(sm->machine->GetCurrentStateName()));
//	int targetIndex = static_cast<int>(targetState);
//
//	if (currentIndex == targetIndex)
//	{
//		return true;
//	}
//
//	if (PlayerTransitions[currentIndex][targetIndex] == 1)
//	{
//		sm->machine->ChangeState(stateName, entity);
//		return true;
//	}
//
//	return false;
//}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	GameManager* gameManager = GameManager::GInstance();
	App app;
	app.Initialize(gameManager);
	//StateMachineComponent* sm = componentManager->AddComponent<StateMachineComponent>(id);
	//sm->Init();
	//sm->machine->RegisterState("IDLE", new IdleState());
	//sm->machine->RegisterState("RIGHT", new RightState());
	//sm->machine->RegisterState("LEFT", new LeftState());

	//sm->machine->ChangeState("IDLE", id);

	gameManager->Run();
	return 0;
}