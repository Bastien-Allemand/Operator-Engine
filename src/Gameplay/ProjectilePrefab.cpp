#include "pch.h"
#include "ProjectilePrefab.h"
#include "Component/ColliderComponent.h"
#include "Component/InputComponent.h"
#include "Component/MeshComponent.h"


#include "IRenderer.h"
#include "BulletComponent.h"
#include "TypeProjectileComponent.h"

#include "Manager/ComponentManager.h"

ProjectilePrefab::ProjectilePrefab(Mesh* _sphere, Texture* _texture, ProjectileType _type)
{
	transform = componentManager->AddComponent<TransformComponent>(id);
	transform->localTransform.scale = { 0.05f, 0.05f, 0.05f };
	ColliderComponent* collider = componentManager->AddComponent<ColliderComponent>(id);
	collider->flag = PROJECTILE;
	collider->isTrigger = true;
	collider->colliderType = ColliderComponent::SPHERE;

	DirectX::BoundingSphere* sphere = new DirectX::BoundingSphere(transform->localTransform.position, 0.05f);
	collider->boundSphere = sphere;

	//Geometry* geometry = new Geometry();
	//geometry->BuildSphere(100, 100);

	//MeshComponent* mesh = componentManager->AddComponent<MeshComponent>(id);

	TypeProjectileComponent* typeProjectile = componentManager->AddComponent<TypeProjectileComponent>(id);
	typeProjectile->type = _type;




	MeshComponent* mesh = componentManager->AddComponent<MeshComponent>(id);
	mesh->mesh = _sphere;
	mesh->texture = _texture;




	//mesh->texturePath = L"../../res/Render/Test.dds";

	BulletComponent* bulletComp = componentManager->AddComponent<BulletComponent>(id);

}
ProjectilePrefab::~ProjectilePrefab()
{
}
