#include "pch.h"
#include "EnnemyPrefab.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "Manager/SystemManager.h"


#include "Component/ColliderComponent.h"
#include "Component/MeshComponent.h"

#include "HealthComponent.h"
#include "EnemyIAComponent.h"
#include "ShootComponent.h"

#include "IRenderer.h"
#include "Geometry.h"

EnnemyPrefab::EnnemyPrefab(Mesh* _mesh, Texture* _texture)
{
	transform = componentManager->AddComponent<TransformComponent>(id);
	transform->localTransform.scale = Vector3f(0.25f, 0.25f, 0.25f);
	ColliderComponent* collider = componentManager->AddComponent<ColliderComponent>(id);
	collider->flag = ENEMY;
	collider->isTrigger = true;
	collider->colliderType = ColliderComponent::SPHERE;
	DirectX::BoundingSphere* sphere = new DirectX::BoundingSphere(transform->localTransform.position, 0.25f);
	collider->boundSphere = sphere;


	Geometry geometry;
	geometry.BuildSphere(20,20);
	gameManager->GRenderEngine()->InitMesh(*_mesh, geometry);


	MeshComponent* mesh = componentManager->AddComponent<MeshComponent>(id);
	mesh->mesh = _mesh;
	mesh->texture = _texture;	


	HealthComponent* health = componentManager->AddComponent<HealthComponent>(id);
	health->health = 5;
	health->maxHealth = 5;

	EnemyIAComponent* enemyIA = componentManager->AddComponent<EnemyIAComponent>(id);
	ShootComponent* shootComp = componentManager->AddComponent<ShootComponent>(id);
	shootComp->canShoot = false;
	shootComp->fireRate = 5.0f;
}

EnnemyPrefab::~EnnemyPrefab()
{
}
