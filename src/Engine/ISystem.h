#pragma once
class GameManager;
class ISystem
{
public:
	~ISystem() = default;
	virtual void Init(GameManager* _gameManager) = 0;
	virtual void Update(GameManager* _gameManager) = 0;
	virtual void AddEntity(uint32 _entityId,size_t _hashCode);
	virtual void RemoveEntity(uint32 _entityId);
	virtual void RemoveComponent(uint32 _entityId, size_t _hashCode);
protected:
	Vector<uint32> m_entities;
	size_t m_hashCode = 0;
};

