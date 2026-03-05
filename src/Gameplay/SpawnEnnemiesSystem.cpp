#include "pch.h"
#include "SpawnEnnemiesSystem.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"

#include "Component/TransformComponent.h"
#include "Component/MeshComponent.h"
#include "Component/ColliderComponent.h"

#include "Geometry.h"
#include "IRenderer.h"
#include "Mesh.h"

#include "HealthComponent.h"

#include "EnnemyPrefab.h"
SpawnEnnemiesSystem::SpawnEnnemiesSystem()
{

}

void SpawnEnnemiesSystem::Init(GameManager* _gameManager)
{
	IRenderer* irenderer = _gameManager->GRenderEngine();
	m_mesh = new Mesh();
	m_ennemyTexture = irenderer->LoadTexture(L"../../res/Render/splash.dds");
	Geometry geometry;
	geometry.BuildSphere(20, 20);
	irenderer->InitMesh(*m_mesh, geometry);
}

void SpawnEnnemiesSystem::Update(GameManager* _gameManager)
{	
	ComponentManager* componentManager = _gameManager->GComponentManager();
	m_spawnTimer += _gameManager->GCDeltaTime();
	int index = 0;
	for (uint32 entity : m_enemies)
	{

		HealthComponent* health = componentManager->GComponent<HealthComponent>(entity);
		if (health->health<=0)
		{
			m_recycleEntities.push_back(entity);
			m_enemies.erase(m_enemies.begin()+index);
		}
		index++;
	}

	if (m_spawnTimer >= m_spawnRate)
	{
		if (m_enemies.size() < m_maxEnnemies)
		{
			//Recycle
			if (m_recycleEntities.size() > 0)
			{
				uint32 entity = m_recycleEntities[0];
				HealthComponent* health = componentManager->GComponent<HealthComponent>(entity);
				health->health = health->maxHealth;

				MeshComponent* mesh = componentManager->GComponent<MeshComponent>(entity);
				mesh->visible = true;
				ColliderComponent* collider = componentManager->GComponent<ColliderComponent>(entity);
				collider->isTrigger = true;

				TransformComponent* transform = componentManager->GComponent<TransformComponent>(entity);
				transform->localTransform.position = Vector3f(	rand() % 10 - 5, 
																0.1f,
																rand() % 10 + 5);
				m_enemies.push_back(m_recycleEntities[0]); 
				m_recycleEntities.erase(m_recycleEntities.begin());

			}
			else
			{
				EnnemyPrefab newEnnemy(m_mesh, m_ennemyTexture);
				newEnnemy.transform->localTransform.position = Vector3f(rand() % 6 - 3,
					0.1f,
					rand() % 10 + 5);
				m_enemies.push_back(newEnnemy.GID());
			}
			m_spawnTimer = 0.0f;
			m_changeSpawnRate = true;
		}
		else
		{
			m_spawnTimer = 0.0f;
			m_changeSpawnRate = true;
		}
	}
	if (m_changeSpawnRate)	m_spawnRate = rand() % 20 + 10;
}
