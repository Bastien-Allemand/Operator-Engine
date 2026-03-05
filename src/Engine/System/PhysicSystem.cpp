#include "pch.h"
#include "PhysicSystem.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"

#include "Component/PhysicComponent.h"
#include "Component/transformComponent.h"
#include "Component/ColliderComponent.h"

PhysicSystem::PhysicSystem()
{
	m_hashCode = typeid(PhysicComponent).hash_code();
}
void PhysicSystem::Init(GameManager* _gameManager)
{
	m_componentManager = _gameManager->GComponentManager();
}	

void PhysicSystem::Update(GameManager* _gameManager)
{
	Vector<ColliderComponent*> colliders;
	Vector<TransformComponent*> transforms;
	Vector<PhysicComponent*> physics;
	Vector<uint32> entities;

	//Setup Vectors and apply to move to targets 
	for (uint32 entity : m_entities)
	{
		physics.push_back(m_componentManager->GComponent<PhysicComponent>(entity));
		transforms.push_back(m_componentManager->GComponent<TransformComponent>(entity));
		colliders.push_back(m_componentManager->GComponent<ColliderComponent>(entity));
		entities.push_back(entity);

		if (physics.back()->targets.size() > 0)
		{
			Vector3f Targetpos = { physics.back()->targets[0].x,physics.back()->targets[0].y,physics.back()->targets[0].z};
			Vector3f distance;
			distance.x = std::abs(Targetpos.x - transforms.back()->localTransform.position.x);
			distance.y = std::abs(Targetpos.y - transforms.back()->localTransform.position.y);
			distance.z = std::abs(Targetpos.z - transforms.back()->localTransform.position.z);

			if (distance.x < 0.1f && distance.y < 0.1f && distance.z < 0.1f)
			{
				physics.back()->targets.erase(physics.back()->targets.begin());
			}
			else
			{
				Vector3f Target = { physics.back()->targets[0].x,physics.back()->targets[0].y,physics.back()->targets[0].z };
				Vector3f direction = Target - transforms.back()->localTransform.position;
				float speed = physics.back()->targets[0].w;
				transforms.back()->localTransform.position += direction * speed * _gameManager->GCDeltaTime();
			}
		}
	}
	// ----------- APPLY FORCES AND MOVE -----------
	for (int i = 0; i < entities.size(); i++)
	{
		PhysicComponent* physic = physics[i];
		TransformComponent* transform = transforms[i];

		if (!physic || !transform || !physic->isKinetic) continue;

		// ----- Apply total forces -----
		Vector3f totalForce = physic->mainForce;

		// Move transform
		transform->localTransform.position += totalForce * _gameManager->GCDeltaTime();

	}
	//Collision Detection and response
	for (int i = 0; i < entities.size(); i++)
	{
		ColliderComponent* collider = colliders[i];
		if (collider->collideWith.empty()) continue;

		PhysicComponent* physic = physics[i];
		TransformComponent* transform = transforms[i];

		if (!physic || !transform) continue;

		for (int i = 0; i < collider->collideWith.size(); i++)
		{
			TransformComponent* otherTransform = m_componentManager->GComponent<TransformComponent>(collider->collideWith[i]);
			ColliderComponent* otherCollider = m_componentManager->GComponent<ColliderComponent>(collider->collideWith[i]);
			PhysicComponent* otherPhysic = otherTransform ? m_componentManager->GComponent<PhysicComponent>(collider->collideWith[i]) : nullptr;

			if (!otherTransform || !otherCollider) continue;

			// Use the normal function from the collider (or approximate with delta)
			Vector3f collisionNormal = collider->normals[i];
			if (collisionNormal.x == 0 && collisionNormal.y == 0 && collisionNormal.z == 0)
			{
				// fallback if normal is zero
				Vector3f delta = otherTransform->localTransform.position - transform->localTransform.position;
				collisionNormal = Normalize(delta);
			}
			Vector3f pushOut = collider->GetPenetrationVector(otherCollider);
			transform->localTransform.position += pushOut;
		}

		collider->collideWith.clear();
	}
}
