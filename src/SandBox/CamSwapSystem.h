#pragma once
#include "ISystem.h"
struct CameraComponent;
struct TransformComponent;
struct TextComponent;
struct MeshComponent;

class Mesh;
class Texture;
class ComponentManager;
class InputManager;

class CamSwapSystem : public ISystem
{
	public:
	CamSwapSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
	uint32 playerid = MAX_VALUE;
private:
	bool m_camMode = false;
	float m_animationTimer = 0;
	Vector<CameraComponent*> m_cameras;
	int m_currentCameraIndex = 0;

	Mesh* m_camMesh = nullptr;
	Texture* m_camTexture = nullptr;
	TextComponent* m_textComponent = nullptr;
	ComponentManager* m_componentManager;
	InputManager* m_inputManager;

	uint32 m_phoneId = MAX_VALUE;
	Mesh* m_phoneMesh = nullptr;
	TransformComponent* m_phoneTransform = nullptr;
	MeshComponent* m_phonecompMesh = nullptr;

};

