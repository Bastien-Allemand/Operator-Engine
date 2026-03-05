#include "pch.h"
#include "App.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "Manager/SystemManager.h"

#include "IRenderer.h"

#include "Inputs.h"

#include "Component/CameraComponent.h"
#include "Component/TransformComponent.h"
#include "Component/InputComponent.h"
#include "Component/ColliderComponent.h"
#include "Component/PhysicComponent.h"

#include "Component/ParticleComponent.h"
#include "Component/ParticleEmitterComponent.h"

#include "Component/LightComponent.h"
#include "Cbuffers.h"

#include "ProjectilSystem.h"

#include "MovementSystem.h"
#include "MovementComponent.h"

#include "Font.h"
#include "Component/UITransformComponent.h"
#include "Component/UIImageComponent.h"
#include "Component/TextComponent.h"

#include "Component/PostProcessComponent.h"

#include "Mesh.h"
#include "Geometry.h"
#include "Component/MeshComponent.h"

void App::Initialize(GameManager* _gameManager)
{
	SystemManager* systemManager = _gameManager->GSystemManager();
	systemManager->AddSystem(new MovementSystem());
	systemManager->AddSystem(new ProjectilSystem());
	//init meshs and texture
	Geometry m_geometry2;
	m_geometry2.BuildPlane(5, 5);
	m_mesh2 = new Mesh();
	_gameManager->GRenderEngine()->InitMesh(*m_mesh2, m_geometry2);

	Geometry m_geometry;
	m_geometry.BuildBox(1, 1, 1);
	m_mesh = new Mesh();
	_gameManager->GRenderEngine()->InitMesh(*m_mesh, m_geometry);

	Geometry m_geometry3;
	m_geometry3.BuildSphere(16, 16,1);
	m_mesh3 = new Mesh();
	_gameManager->GRenderEngine()->InitMesh(*m_mesh3, m_geometry3);

	Geometry m_geometry4;
	m_geometry4.BuildCylinder(16, 1);
	m_mesh4 = new Mesh();
	_gameManager->GRenderEngine()->InitMesh(*m_mesh4, m_geometry4);

	m_texture = _gameManager->GRenderEngine()->LoadTexture(L"../../res/Render/Test.dds");
	//camera 
	uint32 Cameraid = _gameManager->GNewEntityId();
	TransformComponent* transform = _gameManager->GComponentManager()->AddComponent<TransformComponent>(Cameraid);
	transform->localTransform.position = Vector3f(5, -9, 0);
	CameraComponent* camera = _gameManager->GComponentManager()->AddComponent<CameraComponent>(Cameraid);
	camera->swapTothisCamera = true;
	MovementComponent* movement = _gameManager->GComponentManager()->AddComponent<MovementComponent>(Cameraid);
	movement->isActive = true;
	movement->lockCursor = true;
	DirectX::BoundingBox playerHitbox;
	playerHitbox.Center = { 0,0,0 };
	playerHitbox.Extents = { 0.5f,1,0.5f };
	IInputComponent* input = _gameManager->GComponentManager()->AddComponent<IInputComponent>(Cameraid);
	{
	input->actions.push_back(MoveLeft);
	input->actions.push_back(MoveRight);
	input->actions.push_back(MoveFoward);
	input->actions.push_back(MoveBackward);
	input->actions.push_back(MoveUp);
	input->actions.push_back(MoveDown);
	input->actions.push_back(TurnLeft);
	input->actions.push_back(TurnRight);
	input->actions.push_back(TurnUp);
	input->actions.push_back(TurnDown);
	input->actions.push_back(SlowDown);
	input->actions.push_back(Mouselock);
	input->actions.push_back(SpeedUp);
	input->actions.push_back(SpawnProjectil);

	input->conditions.push_back({ Condition::DOWN, 'W', 2 });
	input->conditions.push_back({ Condition::DOWN, 'Z', 2 });
	input->conditions.push_back({ Condition::DOWN, 'A', 0 });
	input->conditions.push_back({ Condition::DOWN, 'Q', 0 });
	input->conditions.push_back({ Condition::DOWN, 'S', 3 });
	input->conditions.push_back({ Condition::DOWN, 'D', 1 });
	input->conditions.push_back({ Condition::DOWN, VK_SPACE, 4 });
	input->conditions.push_back({ Condition::DOWN, VK_CONTROL, 5 });
	input->conditions.push_back({ Condition::DOWN, VK_LEFT, 6 });
	input->conditions.push_back({ Condition::DOWN, VK_RIGHT, 7 });
	input->conditions.push_back({ Condition::DOWN, VK_UP, 8 });
	input->conditions.push_back({ Condition::DOWN, VK_DOWN, 9 });
	input->conditions.push_back({ Condition::DOWN, 'P', 10 });
	input->conditions.push_back({ Condition::PRESSED, VK_ESCAPE, 11 });
	input->conditions.push_back({ Condition::DOWN, 'O', 12 });
	input->conditions.push_back({ Condition::PRESSED, VK_LBUTTON,13 });
	}

	LightComponent* light = _gameManager->GComponentManager()->AddComponent<LightComponent>(Cameraid);
	light->Type = LightComponent::LightType::Directional;
	m_light = new Light();
	m_light->direction = {0,1,0};
	m_light->strength = Vector3f(1, 1, 1);
	light->LightData = m_light;
	LightComponent* AmbientLight = _gameManager->GComponentManager()->AddComponent<LightComponent>(Cameraid);
	AmbientLight->Type = LightComponent::LightType::Ambient;
	m_light2 = new Light();
	m_light2->strength = Vector3f(0.7f, 0.7f, 0.7f);
	AmbientLight->LightData = m_light2;


	//ground entity
	uint32 id = _gameManager->GNewEntityId();
	TransformComponent* transform2 = _gameManager->GComponentManager()->AddComponent<TransformComponent>(id);
	transform2->localTransform.position = Vector3f(0,-10, 0);
	transform2->localTransform.SYPR(0,0,0);
	MeshComponent* mesh = _gameManager->GComponentManager()->AddComponent<MeshComponent>(id);
	mesh->mesh = m_mesh2;
	mesh->texture = m_texture;

	id = _gameManager->GNewEntityId();
	transform2 = _gameManager->GComponentManager()->AddComponent<TransformComponent>(id);
	transform2->localTransform.position = Vector3f(0, -10, 0);
	transform2->localTransform.SYPR(0, 0, 0);
	MeshComponent* mesh6 = _gameManager->GComponentManager()->AddComponent<MeshComponent>(id);
	mesh6->mesh = m_mesh2;
	mesh6->texture = m_texture;

	ColliderComponent* collider = _gameManager->GComponentManager()->AddComponent<ColliderComponent>(id);
	DirectX::BoundingBox box;
	box.Center = { 0,0,0 };
	box.Extents = { 5,0.5f,5 };
	collider->boundBox = new DirectX::BoundingBox(box);
	collider->isTrigger = true;
	collider->colliderType = ColliderComponent::BOX;

	//cube entity
	id = _gameManager->GNewEntityId();
	transform = _gameManager->GComponentManager()->AddComponent<TransformComponent>(id);
	transform->localTransform.position = Vector3f(0, -5, 0);
	transform->localTransform.SYPR(0, 0, 0);
	mesh = _gameManager->GComponentManager()->AddComponent<MeshComponent>(id);
	mesh->mesh = m_mesh;
	mesh->texture = m_texture;
	collider = _gameManager->GComponentManager()->AddComponent<ColliderComponent>(id);
	box.Center = { 0,0,0 };
	box.Extents = { 0.5f,0.5f,0.5f };
	collider->boundBox = new DirectX::BoundingBox(box);
	collider->isTrigger = true;
	collider->colliderType = ColliderComponent::BOX;
	PhysicComponent* physic = _gameManager->GComponentManager()->AddComponent<PhysicComponent>(id);
	physic->mainForce = { 0, -1.0f, 0 };
	physic->isKinetic = true;

	//sphere entity
	id = _gameManager->GNewEntityId();
	transform = _gameManager->GComponentManager()->AddComponent<TransformComponent>(id);
	transform->localTransform.position = Vector3f(1,0,1);
	transform->localTransform.SYPR(0, 0, 0);
	mesh = _gameManager->GComponentManager()->AddComponent<MeshComponent>(id);
	mesh->mesh = m_mesh3;
	mesh->texture = m_texture;

	collider = _gameManager->GComponentManager()->AddComponent<ColliderComponent>(id);
	DirectX::BoundingSphere sphere;
	sphere.Center = { 0,0,0 };
	sphere.Radius = 0.5f;
	collider->boundSphere = new DirectX::BoundingSphere(sphere);
	collider->isTrigger = true;
	collider->colliderType = ColliderComponent::SPHERE;
	physic = _gameManager->GComponentManager()->AddComponent<PhysicComponent>(id);
	physic->mainForce = { 0, -1.0f, 0 };
	physic->isKinetic = true;

	//cylinder entity
	id = _gameManager->GNewEntityId();
	transform = _gameManager->GComponentManager()->AddComponent<TransformComponent>(id);
	transform->localTransform.position = Vector3f(-1, 0, -1);
	transform->localTransform.SYPR(0, 0, 0);
	mesh = _gameManager->GComponentManager()->AddComponent<MeshComponent>(id);
	mesh->mesh = m_mesh4;
	mesh->texture = m_texture;

	collider = _gameManager->GComponentManager()->AddComponent<ColliderComponent>(id);
	box.Center = { 0,0,0 };
	box.Extents = { 0.5f,0.5f,0.5f };
	collider->boundBox = new DirectX::BoundingBox(box);
	collider->isTrigger = true;
	collider->colliderType = ColliderComponent::BOX;
	physic = _gameManager->GComponentManager()->AddComponent<PhysicComponent>(id);
	physic->isKinetic = true;

	//particule entity
	id = _gameManager->GNewEntityId();
	transform = _gameManager->GComponentManager()->AddComponent<TransformComponent>(id);
	ParticleEmitterComponent* emitter = _gameManager->GComponentManager()->AddComponent<ParticleEmitterComponent>(id);
	emitter->spawnRate = 1000.f;
	emitter->maxParticles = 10000;
	emitter->shape = EmitterShape::SPHERE;
	emitter->m_pathTexture = new WString(L"../../res/Render/Particule.dds");
	emitter->shapeRadius = 100.f;
	emitter->maxVelocity = {2,2,2};
	emitter->minVelocity = { 1,1,1 };
	//emitter2->startColor = { 0,0,1,1 };
	//emitter2->endColor = { 1,0,0,1 };
	emitter->direction = { 0,-1,0 };
	emitter->force = 0.1f;
	ParticleComponent* particule = _gameManager->GComponentManager()->AddComponent<ParticleComponent>(id);
	particule->gpuResource = _gameManager->GRenderEngine()->CreateParticleSystem(emitter->maxParticles);
	particule->isInitialized = true;

	Font* arial = new Font;

	uint32 textId = _gameManager->GNewEntityId();
	UITransformComponent* uiTransform = _gameManager->GComponentManager()->AddComponent<UITransformComponent>(textId);
	TextComponent* text = _gameManager->GComponentManager()->AddComponent<TextComponent>(textId);
	text->pathTexture = new WString(L"../../res/Render/arial.dds");
	arial->LoadFontData(text, L"../../res/Render/arial.fnt");
	text->text = L"Hello World!";

	Texture* test = _gameManager->GRenderEngine()->LoadTexture(L"../../res/Render/Test.dds");
	uint32 testId = _gameManager->GNewEntityId();
	UITransformComponent* uiTransformTest = _gameManager->GComponentManager()->AddComponent<UITransformComponent>(testId);
	UIImageComponent* image = _gameManager->GComponentManager()->AddComponent<UIImageComponent>(testId);
	image->texture = test;
	uiTransformTest->anchoredPosition = { 100,100 };
	
	/*id = _gameManager->GNewEntityId();
	PostProcessComponent* postProcess = _gameManager->GComponentManager()->AddComponent<PostProcessComponent>(id);
	postProcess->isActive = true;
	postProcess->shaderPath = new WString(L"../../res/Shaders/Contrast.hlsl");
	postProcess->params0 = { 0.9f, 0, 0, 0 };
	postProcess->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();
	Font* arial = new Font;*/

	/*id = _gameManager->GNewEntityId();
	PostProcessComponent* postProcess = _gameManager->GComponentManager()->AddComponent<PostProcessComponent>(id);
	postProcess->isActive = true;
	postProcess->shaderPath = new WString(L"../../res/Shaders/Saturation.hlsl");
	postProcess->params0 = { .5f, 0, 0, 0 };
	postProcess->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();
	Font* arial = new Font;*/

	/*id = _gameManager->GNewEntityId();
	PostProcessComponent* postProcess = _gameManager->GComponentManager()->AddComponent<PostProcessComponent>(id);
	postProcess->isActive = true;
	postProcess->shaderPath = new WString(L"../../res/Shaders/Luminosity.hlsl");
	postProcess->params0 = { 10.f, 0, 0, 0 };
	postProcess->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();*/
	
	uint32 celId = _gameManager->GNewEntityId();
	PostProcessComponent* postProcessCelShading = _gameManager->GComponentManager()->AddComponent<PostProcessComponent>(celId);
	postProcessCelShading->isActive = true;
	postProcessCelShading->shaderPath = new WString(L"../../res/Shaders/Cel-shading.hlsl");
	postProcessCelShading->params0 = { 6.f, .2f, 0.5f, 0.5f };
	postProcessCelShading->params1 = { 0.5f, 1.f, 1.f, 0.f };
	postProcessCelShading->zOrder = 1;
	postProcessCelShading->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();

	// --- Post-Process 2 : Contrast,lumonositer,saturation (zOrder 1) ---
	uint32 ppId2 = _gameManager->GNewEntityId();
	PostProcessComponent* ppContrast = _gameManager->GComponentManager()->AddComponent<PostProcessComponent>(ppId2);
	ppContrast->isActive = true;
	ppContrast->shaderPath = new WString(L"../../res/Shaders/GameShader.hlsl");
	ppContrast->params0 = { 1.0f, 1.5f, 1.0f, 0.0f };
	ppContrast->zOrder = 0;
	ppContrast->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();

	// --- Post-Process 3 : Blur (zOrder 2 = appliqué en dernier) ---
	uint32 ppId3 = _gameManager->GNewEntityId();
	PostProcessComponent* ppBlur = _gameManager->GComponentManager()->AddComponent<PostProcessComponent>(ppId3);
	ppBlur->isActive = true;
	ppBlur->shaderPath = new WString(L"../../res/Shaders/Blur.hlsl");
	ppBlur->params0 = { 1.f, 0.0f, 0.0f, 0.0f };
	ppBlur->zOrder = 2;
	ppBlur->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();
	ppBlur->isActive = false; // Désactive le flou au départ

	// --- Post-Process 3 : Blur (zOrder 2 = appliqué en dernier) ---
	uint32 ppId4 = _gameManager->GNewEntityId();
	PostProcessComponent* ppBlur2 = _gameManager->GComponentManager()->AddComponent<PostProcessComponent>(ppId4);
	ppBlur2->isActive = true;
	ppBlur2->shaderPath = new WString(L"../../res/Shaders/Blur.hlsl");
	ppBlur2->params0 = { 0.f, 0.0f, 0.0f, 0.0f };
	ppBlur2->zOrder = 3;
	ppBlur2->postProcessResource = _gameManager->GRenderEngine()->CreatePostProcessSystem();
	ppBlur2->isActive = false; // Désactive le flou au départ
}
App::~App()
{
	delete m_mesh;
	delete m_mesh2;
	delete m_mesh3;
	delete m_mesh4;
	delete m_light;
	delete m_light2;
}