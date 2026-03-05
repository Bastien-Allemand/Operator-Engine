#include "pch.h"
#include "MeshSystem.h"

#include "Manager/GameManager.h"
#include "IRenderer.h"

#include "Component/TransformComponent.h"
#include "Component/MeshComponent.h"
#include "Manager/ComponentManager.h"
#include "ComponentPool.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"	

MeshSystem::MeshSystem()
{
	m_hashCode = typeid(MeshComponent).hash_code();
}
void MeshSystem::Init(GameManager* _gameManager)
{
	m_componentManager = _gameManager->GComponentManager();
	m_renderer = _gameManager->GRenderEngine();
}
void MeshSystem::Update(GameManager* _gameManager)
{
	DirectX::BoundingFrustum camFrustum;
	Camera* camera = m_renderer->GCamera();
	// Projection
	DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(
		camera->fov,
		m_renderer->GCAspectRatio(),
		camera->nearPlane,
		camera->farPlane
	);

	// Création du frustum en View Space
	DirectX::BoundingFrustum::CreateFromMatrix(camFrustum, proj);

	// Récupérer la View matrix
	Matrix view = m_renderer->BuildViewMatrix();

	// Calculer son inverse
	Matrix invView = DirectX::XMMatrixInverse(nullptr, view);

	// Transformer le frustum en World Space
	camFrustum.Transform(camFrustum, invView);

	m_renderer->BeginDraw();
	for (uint32 entityId : m_entities)
	{

		TransformComponent*  transformComponent = m_componentManager->GComponent<TransformComponent>(entityId);
		if (transformComponent == nullptr)	continue;

		MeshComponent* meshComponent = m_componentManager->GComponent<MeshComponent>(entityId);

		if (!meshComponent->visible) continue;
		DirectX::BoundingSphere meshBounds = meshComponent->mesh->GBounds();
		meshBounds.Center = transformComponent->BuildworldTransfrom().position;

		if (camFrustum.Intersects(meshBounds))
		{
			m_renderer->Draw(meshComponent->mesh,meshComponent->texture, transformComponent->BuildWorldMatrix());
		}
	}
}
