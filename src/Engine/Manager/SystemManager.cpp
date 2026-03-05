#include "pch.h"
#include "SystemManager.h"
#include "GameManager.h"

#include "System/MeshSystem.h"
#include "System/InputSystem.h"
#include "System/CameraSystem.h"
#include "System/StateMachineSystem.h"
#include "System/UISystem.h"
#include "System/ColliderSystem.h"
#include "System/PhysicSystem.h"
#include "System/TextSystem.h"
#include "System/ParticleSystem.h"
#include "System/PostProcessSystem.h"

#include "IRenderer.h"
#include "System/LightSystem.h"
#include "ISystem.h"
#include "IRenderer.h"

SystemManager::SystemManager(GameManager* _GameManager)
{
	m_gameManager = _GameManager;
	m_systems.push_back(new StateMachineSystem());
	m_systems.push_back(new CameraSystem());
	m_systems.push_back(new InputSystem());
	m_systems.push_back(new ColliderSystem());
	m_systems.push_back(new PhysicSystem());

	//To Be Draw At The End
	m_systems.push_back(new MeshSystem());
	m_systems.push_back(new UISystem());
	m_systems.push_back(new TextSystem());
	m_systems.push_back(new ParticleSystem());
	m_systems.push_back(new LightSystem());
	m_systems.push_back(new PostProcessSystem());
}
void SystemManager::Init()
{
	for (auto system : m_systems)	system->Init(m_gameManager);
}

void SystemManager::AddSystem(ISystem* _system)
{
	m_systems.push_back(_system);
	m_systems.back()->Init(m_gameManager);
}
void SystemManager::Update()
{
	for (auto system : m_systems)	system->Update(m_gameManager);
	m_gameManager->GRenderEngine()->CloseDraw();
}
void SystemManager::AddEntity(uint32 _entityId, size_t _hashCode)
{
	for (auto system : m_systems)	system->AddEntity(_entityId,_hashCode);
}
void SystemManager::RemoveEntity(uint32 _entityId)
{
	for (auto system : m_systems)	system->RemoveEntity(_entityId);
}

void SystemManager::RemoveComponent(uint32 _entityId, size_t _hashCode)
{

}