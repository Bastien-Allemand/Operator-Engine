#include "pch.h"
#include "ProjectilSystem.h"

#include "ProjectilComponent.h"
#include "Component/TransformComponent.h"
#include "Component/MeshComponent.h"
#include "IRenderer.h"
#include "Manager/ComponentManager.h"
#include "Manager/GameManager.h"
#include "Mesh.h"
#include "Geometry.h"

ProjectilSystem::ProjectilSystem()
{
	m_hashCode = typeid(ProjectilComponent).hash_code();
}

void ProjectilSystem::Init(GameManager* _gameManager)
{
	m_projectilMesh = new Mesh();
	Geometry geo;
	geo.BuildSphere(16, 16, 0.5f);
	m_projectilTexture = _gameManager->GRenderEngine()->LoadTexture(L"../../res/Render/Test.dds");
	_gameManager->GRenderEngine()->InitMesh(*m_projectilMesh, geo);
	m_isInit = true;
}

void ProjectilSystem::Update(GameManager* _gameManager)
{
	ComponentManager* componentManager = _gameManager->GComponentManager();
	for (auto entity : m_entities)
	{
		ProjectilComponent* projectilComponent = componentManager->GComponent<ProjectilComponent>(entity);
		if (projectilComponent->toSpawn)
		{
			MeshComponent* meshComponent = componentManager->AddComponent<MeshComponent>(entity);
			meshComponent->mesh = m_projectilMesh;
			meshComponent->texture = m_projectilTexture;
			projectilComponent->toSpawn = false;
		}
		projectilComponent->lifetime -= _gameManager->GCTruDeltaTime();
		if (projectilComponent->lifetime < 0)
		{
			_gameManager->DestroyEntity(entity);
		}
		else
		{
			TransformComponent* transformComponent = componentManager->GComponent<TransformComponent>(entity);
			transformComponent->localTransform.position += projectilComponent->direction * _gameManager->GCDeltaTime();
		}

	}
}
