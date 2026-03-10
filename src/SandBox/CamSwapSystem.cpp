#include "pch.h"
#include "CamSwapSystem.h"

#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "Manager/InputManager.h"

#include "Geometry.h"
#include "IRenderer.h"
#include "Mesh.h"

#include "Component/CameraComponent.h"
#include "Component/TransformComponent.h"
#include "Component/MeshComponent.h"
#include "MovementComponent.h"


CamSwapSystem::CamSwapSystem()
{
}

void CamSwapSystem::Init(GameManager* _gameManager)
{
	m_camMesh = new Mesh();
	Geometry camGeo;
	camGeo.BuildBox(0.2f, 0.2f, 0.2f);
	_gameManager->GRenderEngine()->InitMesh(*m_camMesh, camGeo);
	m_camTexture = _gameManager->GRenderEngine()->LoadTexture(L"../../res/Render/Test.dds");

	m_componentManager = _gameManager->GComponentManager();

	m_inputManager = InputManager::GInstance();
}

void CamSwapSystem::Update(GameManager* _gameManager)
{
	//enter and exite cam Mode
	if (m_inputManager->IsKeyPressed('F') && m_cameras.size() > 0)
	{
		m_camMode = !m_camMode;
		MovementComponent* movement = m_componentManager->GComponent<MovementComponent>(playerid);
		if (m_camMode)
		{
			movement->lockCursor = false;
			for (size_t i = 0; i < m_cameras.size(); i++)
			{
				if (!m_cameras[i]->mainCamera)
				{
					m_cameras[i]->swapTothisCamera = true;
					m_currentCameraIndex = i;
					break;
				}
			}
		}
		else
		{
			movement->lockCursor = true;
			CameraComponent* camera = m_componentManager->GComponent<CameraComponent>(playerid);
			if(camera != nullptr)
				camera->swapTothisCamera = true;

		}
	}

	//event in cam Mode
	if (m_camMode)
	{
		if (m_inputManager->IsKeyPressed(VK_LEFT))
		{
			if (m_cameras.size() > m_currentCameraIndex + 1) 
			{
				m_currentCameraIndex++;
				m_cameras[m_currentCameraIndex]->swapTothisCamera = true;
			}
		}
		if (m_inputManager->IsKeyPressed(VK_RIGHT))
		{
			if (m_currentCameraIndex > 0)
			{
				m_currentCameraIndex--;
				m_cameras[m_currentCameraIndex]->swapTothisCamera = true;
			}
		}
	}
	else
	{
		if (m_inputManager->IsKeyPressed(VK_LBUTTON))
		{
			//spawn a camera on player pos
			TransformComponent* playerTransform = m_componentManager->GComponent<TransformComponent>(playerid);
			if (playerTransform != nullptr)
			{
				uint32 camId = _gameManager->GNewEntityId();
				TransformComponent* transform = m_componentManager->AddComponent<TransformComponent>(camId);
				transform->localTransform = playerTransform->localTransform;

				CameraComponent* camera = m_componentManager->AddComponent<CameraComponent>(camId);
				m_cameras.push_back(camera);
				MeshComponent* mesh = m_componentManager->AddComponent<MeshComponent>(camId);
				mesh->mesh = m_camMesh;
				mesh->texture = m_camTexture;

			}
		}
	}
}
