#pragma once
class MeshSystem;
class GameManager;
class InputSystem;
class CameraSystem;
class ISystem;

class SystemManager
{
public:
	SystemManager(GameManager* _GameManager);
	void AddSystem(ISystem* _system);
	void Init();
	void Update();
	void AddEntity(uint32 _entityId, size_t _hashCode);
	void RemoveEntity(uint32 _entityId);
	void RemoveComponent(uint32 _entityId, size_t _hashCode);
private:
	GameManager* m_gameManager;
	Vector<ISystem*> m_systems;
};