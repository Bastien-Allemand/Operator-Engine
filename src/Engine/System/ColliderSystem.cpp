#include "pch.h"
#include "ColliderSystem.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "ComponentPool.h"
#include "Component/ColliderComponent.h"
#include "Component/transformComponent.h"

ColliderSystem::ColliderSystem()
{
	m_hashCode = typeid(ColliderComponent).hash_code();
}

void ColliderSystem::Init(GameManager* _gameManager)
{
	m_componentManager = _gameManager->GComponentManager();
}

void ColliderSystem::Update(GameManager* _gameManager)
{
    const int chunkSize = 14;
    const int range = 2;

    for (size_t i = 0; i < m_entities.size(); i++)
    {
        ColliderComponent* colliderA = m_componentManager->GComponent<ColliderComponent>(m_entities[i]);
		if (!colliderA || colliderA->isTrigger) continue;
		colliderA->collideWith.clear();
		colliderA->normals.clear();
    }

    for (size_t i = 0; i < m_entities.size(); i++)
    {
        uint32 entityA = m_entities[i];

        TransformComponent* transformA = m_componentManager->GComponent<TransformComponent>(entityA);
        ColliderComponent* colliderA = m_componentManager->GComponent<ColliderComponent>(entityA);

        if (!transformA || !colliderA) continue;
        if (!colliderA->isTrigger) continue;

        Vector3f posA = transformA->localTransform.position;

        int chunkAX = (int)std::floor(posA.x / chunkSize);
        int chunkAY = (int)std::floor(posA.y / chunkSize);
        int chunkAZ = (int)std::floor(posA.z / chunkSize);
        for (size_t j = i + 1; j < m_entities.size(); j++)
        {
            uint32 entityB = m_entities[j];

            TransformComponent* transformB = m_componentManager->GComponent<TransformComponent>(entityB);
            ColliderComponent* colliderB = m_componentManager->GComponent<ColliderComponent>(entityB);

            if (!transformB || !colliderB) continue;
            if (!colliderB->isTrigger) continue;
            if (colliderA->flag != -1 && colliderA->flag == colliderB->flag) continue;

            Vector3f posB = transformB->localTransform.position;

            int chunkBX = (int)std::floor(posB.x / chunkSize);
            int chunkBY = (int)std::floor(posB.y / chunkSize);
            int chunkBZ = (int)std::floor(posB.z / chunkSize);

            int dx = abs(chunkAX - chunkBX);
            int dy = abs(chunkAY - chunkBY);
            int dz = abs(chunkAZ - chunkBZ);

            if (dx >= range || dy >= range || dz >= range) continue;

			colliderA->Updatebound(*transformA);
			colliderB->Updatebound(*transformB);
            //_Log("Collision detected between Entity %u and Entity %u", entityA, entityB);
            if (colliderA->Intersect(*colliderB))
            {
                colliderA->collideWith.push_back(entityB);
                colliderB->collideWith.push_back(entityA);
                //find normals
				colliderA->normals.push_back(colliderA->GetCollisionNormal(colliderB));
				colliderB->normals.push_back(colliderB->GetCollisionNormal(colliderA));
            }
        }
    }
}