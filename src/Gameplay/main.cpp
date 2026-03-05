#include "pch.h"
#include <iostream>
#include "main.h"
#include "Levels.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "Component/TransformComponent.h" 
#include "Core/Transform.h"
#include "Component/MeshComponent.h"
#include "Component/InputComponent.h"
#include "Component/CameraComponent.h"
#include "Component/StateMachineComponent.h"
#include "Component/UIImageComponent.h"
#include "Component/UITransformComponent.h"
#include "Component/ColliderComponent.h"
#include "Component/PhysicComponent.h"
#include "System/ColliderSystem.h"
#include "Render/Geometry.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	GameManager* gameManager = GameManager::GInstance();

	LaunchLevel(gameManager);

	gameManager->Run();
	return 0;
}