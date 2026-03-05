#include "pch.h"
#include "PlayerPrefab.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "Manager/SystemManager.h"
#include "Render/Cbuffers.h"

#include "IRenderer.h"

#include "Inputs.h"

#include "Component/CameraComponent.h"
#include "Component/InputComponent.h"
#include "Component/ParticleComponent.h"
#include "Component/ParticleEmitterComponent.h"
#include "Component/ColliderComponent.h"
#include "Component/LightComponent.h"
#include "Component/TextComponent.h"
#include "Component/UITransformComponent.h"
#include "Mesh.h"

#include "Font.h"

#include "ShootComponent.h"
#include "MoveSystem.h"
#include "MoveComponent.h"
#include "HealthComponent.h"
#include "ScoreComponent.h"
#include "TransitionComponent.h"
#include "StockProjectileComponent.h"

#include "Geometry.h"
#include "Component/MeshComponent.h"

PlayerPrefab::PlayerPrefab(Mesh* _box)
{
	transform = componentManager->AddComponent<TransformComponent>(id);

	LightComponent* light = componentManager->AddComponent<LightComponent>(id);
	light->LightData = new Light();
	light->LightData->strength = Vector3f(0.75f, 0.75f, 0.75f);
	light->LightData->direction = transform->localTransform.forward;

	/*uint64 idCamera = gameManager->GNewEntityId();
	TransformComponent* transformCamera = componentManager->AddComponent<TransformComponent>(idCamera);*/
	CameraComponent* camera = componentManager->AddComponent<CameraComponent>(id);
	MoveComponent* movement = componentManager->AddComponent<MoveComponent>(id);
	movement->isActive = true;
	movement->lockCursor = true;


	TransitionComponent* transition = componentManager->AddComponent<TransitionComponent>(id);

	ColliderComponent* collider = componentManager->AddComponent<ColliderComponent>(id);

	collider->flag = PLAYER;
	DirectX::BoundingBox* box = new DirectX::BoundingBox(transform->localTransform.position, Vector3f(1, 1, 1));
	collider->boundBox = box;
	collider->isTrigger = true;
	collider->colliderType = ColliderComponent::BOX;


	IInputComponent* input = componentManager->AddComponent<IInputComponent>(id);


	input->actions.push_back(MoveLeft);
	input->actions.push_back(MoveRight);
	input->actions.push_back(MoveFoward);
	input->actions.push_back(MoveBackward);
	input->actions.push_back(MoveUp);
	input->actions.push_back(MoveDown);
	input->actions.push_back(Slow);
	input->actions.push_back(Boost);
	input->actions.push_back(Normal);
	input->actions.push_back(Shoot);
	input->actions.push_back(TogglePostProcess);
	input->actions.push_back(Pause);

	input->conditions.push_back({ Condition::DOWN, VK_LEFT, 0 });
	input->conditions.push_back({ Condition::DOWN, VK_RIGHT, 1 });
	input->conditions.push_back({ Condition::DOWN, VK_UP, 2 });
	input->conditions.push_back({ Condition::DOWN, VK_DOWN, 3 });
	input->conditions.push_back({ Condition::DOWN, VK_SPACE, 4 });
	input->conditions.push_back({ Condition::DOWN, VK_CONTROL, 5 });
	input->conditions.push_back({ Condition::DOWN, 'S', 6 });
	input->conditions.push_back({ Condition::DOWN, 'Z', 7 });
	input->conditions.push_back({ Condition::RELEASED, 'S', 8 });
	input->conditions.push_back({ Condition::RELEASED, 'Z', 8 });
	input->conditions.push_back({ Condition::DOWN, VK_LBUTTON, 9 });
	input->conditions.push_back({ Condition::PRESSED, VK_F5, 10 });
	input->conditions.push_back({ Condition::PRESSED, VK_ESCAPE, 11 });

	HealthComponent* health = componentManager->AddComponent<HealthComponent>(id);
	health->health = 20;
	health->maxHealth = 20;

	ShootComponent* shootComp = componentManager->AddComponent<ShootComponent>(id);

	camera->swapTothisCamera = true;

	Geometry geometry;
	geometry.BuildBox(1, 1, 1);
	gameManager->GRenderEngine()->InitMesh(*_box, geometry);

	MeshComponent* mesh = componentManager->AddComponent<MeshComponent>(id);
	mesh->mesh = _box;
	mesh->texture = gameManager->GRenderEngine()->LoadTexture(L"../../res/Render/bullet.dds");


	ScoreComponent* score = componentManager->AddComponent<ScoreComponent>(id);


	UITransformComponent* uiTransform = componentManager->AddComponent<UITransformComponent>(id);


	uiTransform->size = Vector2f(200, 50);
	uiTransform->anchoredPosition = Vector2f(-850, -500);


	m_font = new Font;

	TextComponent* text = componentManager->AddComponent<TextComponent>(id);
	text->pathTexture = new WString(L"../../res/Render/arial.dds");
	m_font->LoadFontData(text, L"../../res/Render/arial.fnt");
	text->text = L"Score: " + std::to_wstring(score->score) + L" / Life: " + std::to_wstring(health->health) + L"/" + std::to_wstring(health->maxHealth);


	StockProjectileComponent* stockProjectile = componentManager->AddComponent<StockProjectileComponent>(id);
	IRenderer* irenderer = gameManager->GRenderEngine();
	stockProjectile->m_mesh = new Mesh();
	stockProjectile->m_texture = irenderer->LoadTexture(L"../../res/Render/bullet.dds");
	Geometry geometryProjectile;
	geometryProjectile.BuildSphere(20, 20);
	irenderer->InitMesh(*stockProjectile->m_mesh, geometryProjectile);
}

PlayerPrefab::~PlayerPrefab() {}