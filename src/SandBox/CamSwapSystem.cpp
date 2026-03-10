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

#include "Component/UITransformComponent.h"
#include "Component/TextComponent.h"
#include "Font.h"


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

	Font* arial = new Font;

	uint32 textId = _gameManager->GNewEntityId();
	UITransformComponent* uiTransform = _gameManager->GComponentManager()->AddComponent<UITransformComponent>(textId);
	uiTransform->anchorMin = { 0.1f,0.1f };
	uiTransform->anchorMax = { 0.1f,0.1f };
	m_textComponent = _gameManager->GComponentManager()->AddComponent<TextComponent>(textId);
	m_textComponent->pathTexture = new WString(L"../../res/Render/arial.dds");
	arial->LoadFontData(m_textComponent, L"../../res/Render/arial.fnt");

	uint32 phoneId = _gameManager->GNewEntityId();
	m_phoneTransform = _gameManager->GComponentManager()->AddComponent<TransformComponent>(phoneId);
	m_phoneTransform->localTransform.position = Vector3f(1,-5,2);
	m_phonecompMesh = _gameManager->GComponentManager()->AddComponent<MeshComponent>(phoneId);
	m_phoneMesh = new Mesh();
	Geometry phoneGeo;
	phoneGeo.BuildBox(0.25f, 0.12f, 0.001f);
	_gameManager->GRenderEngine()->InitMesh(*m_phoneMesh, phoneGeo);
	m_phonecompMesh->mesh = m_phoneMesh;
	m_phonecompMesh->texture = m_camTexture;
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
			m_animationTimer = 2.f;
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
			m_textComponent->text = L"Cam :" + std::to_wstring(m_currentCameraIndex);
		}
		else
		{
			movement->lockCursor = true;
			CameraComponent* camera = m_componentManager->GComponent<CameraComponent>(playerid);
			if(camera != nullptr)
				camera->swapTothisCamera = true;
			m_textComponent->text = L"NoCam";
		}
	}
	if (m_inputManager->IsKeyPressed('X'))
	{
		TransformComponent* playerTransform = m_componentManager->GComponent<TransformComponent>(playerid);
		m_phoneTransform->localTransform.position = playerTransform->localTransform.position + playerTransform->localTransform.forward  * 0.2f;
		m_phoneTransform->localTransform.SrotMatrix(playerTransform->localTransform.rotmatrix);
	}
	//event in cam Mode
	if (m_camMode && m_animationTimer <= 0)
	{
		if (m_inputManager->IsKeyPressed(VK_LEFT))
		{
			if (m_cameras.size() > m_currentCameraIndex + 1) 
			{
				m_currentCameraIndex++;
				m_cameras[m_currentCameraIndex]->swapTothisCamera = true;
			}
			m_textComponent->text = L"Cam :" + std::to_wstring(m_currentCameraIndex);
		}
		if (m_inputManager->IsKeyPressed(VK_RIGHT))
		{
			if (m_currentCameraIndex > 0)
			{
				m_currentCameraIndex--;
				m_cameras[m_currentCameraIndex]->swapTothisCamera = true;
			}
			m_textComponent->text = L"Cam :" + std::to_wstring(m_currentCameraIndex);
		}
	}
	else
	{
		if (m_inputManager->IsKeyPressed(VK_LBUTTON) && m_animationTimer <= 0)
		{
			//spawn a camera on player pos
			TransformComponent* playerTransform = m_componentManager->GComponent<TransformComponent>(playerid);
			if (playerTransform != nullptr)
			{
				uint32 camId = _gameManager->GNewEntityId();
				TransformComponent* transform = m_componentManager->AddComponent<TransformComponent>(camId);
				transform->localTransform.position = playerTransform->localTransform.position;
				transform->localTransform.SrotMatrix(playerTransform->localTransform.rotmatrix);

				CameraComponent* camera = m_componentManager->AddComponent<CameraComponent>(camId);
				m_cameras.push_back(camera);
				MeshComponent* mesh = m_componentManager->AddComponent<MeshComponent>(camId);
				mesh->mesh = m_camMesh;
				mesh->texture = m_camTexture;
			}
		}
	}

	if (m_animationTimer > 0)
	{
		m_animationTimer -= _gameManager->GCDeltaTime();
		TransformComponent* playerTransform = m_componentManager->GComponent<TransformComponent>(playerid);
		Vector3f addpos = VEC3_ZERO + m_animationTimer;
		m_phoneTransform->localTransform.position = playerTransform->localTransform.position + playerTransform->localTransform.forward * 0.2f + addpos;
		m_phoneTransform->localTransform.SrotMatrix(playerTransform->localTransform.rotmatrix);
	}
}
