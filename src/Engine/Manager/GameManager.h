#pragma once
#include <chrono>

class Mesh;
class Window;
class InputManager;
class IRenderer;
class SystemManager;
class ComponentManager;

class GameManager
{
public:
	static GameManager* GInstance();
	~GameManager();

	void Run();

	ComponentManager* GComponentManager() { return m_componentManager; }
	SystemManager* GSystemManager() { return m_systemManager; }
	IRenderer* GRenderEngine() { return m_renderer; }
	Window* GWindow() { return m_window; }

	float GCFps() { return m_fps; }

	float GCDeltaTime() { return m_deltaTime * m_speedTime; }
	float GCSpeedTime() { return m_speedTime; }

	float GCTruDeltaTime() { return m_deltaTime; }

	float GTimer() { return m_time; }

	uint32 GNewEntityId();
	void DestroyEntity(uint32 _entityId);
	void SSpeedTime(float _newSpeed) { m_speedTime = _newSpeed; }
	Vector2i GMousePosition();
	Vector3f GMouseWorldPosition();
	void SMousePosition(Vector2i _newPos); 

private:
	GameManager();
	static GameManager* m_instance;
	std::chrono::steady_clock::time_point m_lastTime;
	float m_timer = 1;
	float m_fps = 0;
	float m_deltaTime = 0;
	float m_time = 0;
	float m_speedTime = 1;
	float m_splashTimer = 3;

	Vector<uint32> m_entityIds;
	Vector<uint32> m_freeEntityIds;
	
	SystemManager* m_systemManager;
	ComponentManager* m_componentManager;
	InputManager* m_inputManager;

	Window* m_window;
	IRenderer* m_renderer;

};

