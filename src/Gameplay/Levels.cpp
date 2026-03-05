#include "pch.h"
#include "Levels.h"
#include "ProjectilePrefab.h"
#include "PlayerPrefab.h"
#include "EnnemyPrefab.h"
#include "RailPrefab.h"
#include "MoveSystem.h"
#include "BulletMovementSystem.h"
#include "ShootEnemySystem.h"
#include "HealthSystem.h"
#include "ShootTimerSystem.h"
#include "SpawnEnnemiesSystem.h"
#include "TrackGenerator.h"
#include "Manager/ComponentManager.h"
#include "Manager/SystemManager.h"
#include "RailSystem.h"
#include "Mesh.h"
#include "Component/PostProcessComponent.h"
#include "IRenderer.h"

void LaunchLevel(GameManager* _gameManager)
{
	srand(time(NULL));
	SystemManager* systemManager = _gameManager->GSystemManager();
	systemManager->AddSystem(new MoveSystem());
	systemManager->AddSystem(new HealthSystem());
	systemManager->AddSystem(new ShootTimerSystem());
	systemManager->AddSystem(new ShootEnemySystem());
	systemManager->AddSystem(new SpawnEnnemiesSystem());
	systemManager->AddSystem(new RailSystem());
	systemManager->AddSystem(new BulletMovementSystem());

	ComponentManager* componentManager = _gameManager->GComponentManager();
	
	Mesh* mesh = new Mesh();
	PlayerPrefab player(mesh);
	player.transform->localTransform.position = Vector3f(0, 0.1f, 0);

	// --- Post-Process 1 : Cel-shading (zOrder 0 = appliqué en premier) ---
	uint32 ppId1 = _gameManager->GNewEntityId();
	PostProcessComponent* ppCel = componentManager->AddComponent<PostProcessComponent>(ppId1);
	ppCel->isActive = true;
	ppCel->shaderPath = new WString(L"../../res/Shaders/Cel-shading.hlsl");
	ppCel->params0 = { 6.0f, 0.2f, 0.0f, 0.0f };
	ppCel->params1 = { 0.0f, 1.0f, 0.8f, 0.0f };
	ppCel->zOrder = 1;
	ppCel->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();
	ppCel->isActive = false; // Désactive le cel-shading au départ

	// --- Post-Process 2 : Contrast,lumonositer,saturation (zOrder 1) ---
	uint32 ppId2 = _gameManager->GNewEntityId();
	PostProcessComponent* ppContrast = componentManager->AddComponent<PostProcessComponent>(ppId2);
	ppContrast->isActive = true;
	ppContrast->shaderPath = new WString(L"../../res/Shaders/GameShader.hlsl");
	ppContrast->params0 = { 1.0f, 1.5f, 1.0f, 0.0f };
	ppContrast->zOrder = 0;
	ppContrast->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();
	ppContrast->isActive = false; // Désactive le contraste au départ

	// --- Post-Process 3 : Blur (zOrder 2 et 3 pour horizontal et vertical = appliqué en dernier) --- // need fix
	uint32 ppId3 = _gameManager->GNewEntityId();
	PostProcessComponent* ppBlur = componentManager->AddComponent<PostProcessComponent>(ppId3);
	ppBlur->isActive = true;
	ppBlur->shaderPath = new WString(L"../../res/Shaders/Blur.hlsl");
	ppBlur->params0 = { 1.0f, 0.0f, 0.0f, 0.0f };
	ppBlur->zOrder = 2;
	ppBlur->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();
	ppBlur->isActive = false; // Désactive le flou au départ

	uint32 ppId4 = _gameManager->GNewEntityId();
	PostProcessComponent* ppBlur2 = componentManager->AddComponent<PostProcessComponent>(ppId4);
	ppBlur2->isActive = true;
	ppBlur2->shaderPath = new WString(L"../../res/Shaders/Blur.hlsl");
	ppBlur2->params0 = { .0f, 0.0f, 0.0f, 0.0f };
	ppBlur2->zOrder = 3;
	ppBlur2->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();
	ppBlur2->isActive = false; // Désactive le flou au départ
}
