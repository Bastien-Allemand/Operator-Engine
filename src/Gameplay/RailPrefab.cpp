#include "pch.h"
#include "RailPrefab.h"

#include "Texture.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"

#include "Component/ColliderComponent.h"
#include "Component/MeshComponent.h"

#include "Geometry.h"


RailPrefab::RailPrefab(Mesh* _box, Mesh* _cylinder, Texture* _texture)
{

	SystemManager* systemManager = gameManager->GSystemManager();
	GameManager* gameManager = GameManager::GInstance();
	ComponentManager* componentManager = gameManager->GComponentManager();

	transform = componentManager->AddComponent<TransformComponent>(id);
	transform->localTransform.scale = { 0.1f, 0.1f, 0.1f };

	ColliderComponent* collider = componentManager->AddComponent<ColliderComponent>(id);
	collider->boundBox = new DirectX::BoundingBox(transform->localTransform.position, Vector3f(1, 1, 1));

	// Cylindre 1
	uint32 cyl1_id = gameManager->GNewEntityId();
	m_childEntities.push_back(cyl1_id);

	TransformComponent* cyl1_transform = componentManager->AddComponent<TransformComponent>(cyl1_id);
	cyl1_transform->parent = transform;
	transform->children.push_back(cyl1_transform);
	cyl1_transform->localTransform.AddYPR(0, DirectX::XM_PI * 0.5, 0);
	cyl1_transform->localTransform.position = { -0.5f, 0.0f, 0.0f };
	cyl1_transform->localTransform.scale = { 0.25f, 1.f, 0.25f };
	MeshComponent* cyl1_mesh = componentManager->AddComponent<MeshComponent>(cyl1_id);	
	cyl1_mesh->mesh = _cylinder;
	cyl1_mesh->texture = _texture;	
	// Cylindre 2
	uint32 cyl2_id = gameManager->GNewEntityId();
	m_childEntities.push_back(cyl2_id);

	TransformComponent* cyl2_transform = componentManager->AddComponent<TransformComponent>(cyl2_id);
	cyl2_transform->parent = transform;
	transform->children.push_back(cyl2_transform);
	cyl2_transform->localTransform.AddYPR(0, DirectX::XM_PI * 0.5, 0);
	cyl2_transform->localTransform.position = { 0.5f, 0.0f, 0.0f };
	cyl2_transform->localTransform.scale = { 0.25f, 1.f, 0.25f };
	MeshComponent* cyl2_mesh = componentManager->AddComponent<MeshComponent>(cyl2_id);
	cyl2_mesh->mesh = _cylinder;
	cyl2_mesh->texture = _texture;

	// Box 1
	uint32 box1_id = gameManager->GNewEntityId();
	m_childEntities.push_back(box1_id);

	TransformComponent* box1_transform = componentManager->AddComponent<TransformComponent>(box1_id);
	box1_transform->parent = transform;
	transform->children.push_back(box1_transform);
	box1_transform->localTransform.position = { 0.f, 0.f, 0.25f };
	box1_transform->localTransform.scale = { 0.25f, 0.25f, 0.1f };
	MeshComponent* box1_mesh = componentManager->AddComponent<MeshComponent>(box1_id);
	box1_mesh->mesh = _box;
	box1_mesh->texture = _texture;
	// Box 2
	uint32 box2_id = gameManager->GNewEntityId();
	m_childEntities.push_back(box2_id);

	TransformComponent* box2_transform = componentManager->AddComponent<TransformComponent>(box2_id);
	box2_transform->parent = transform;
	transform->children.push_back(box2_transform);
	box2_transform->localTransform.position = { 0.0f, 0.f, -0.25f };
	box2_transform->localTransform.scale = { 0.5f, 0.5f, 0.1f };
	MeshComponent* box2_mesh = componentManager->AddComponent<MeshComponent>(box2_id);
	box2_mesh->mesh = _box;
	box2_mesh->texture = _texture;
}

RailPrefab::~RailPrefab()
{

}