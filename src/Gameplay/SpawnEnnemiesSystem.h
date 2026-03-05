#pragma once
#include "ISystem.h"

class Texture;
class Mesh;

class SpawnEnnemiesSystem : public ISystem
{
public:
	SpawnEnnemiesSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
private:
	float m_spawnTimer = 0.f;
	float m_spawnRate = 0.5f;

	int m_maxEnnemies = 5;
	Vector<uint32> m_enemies;
	Vector<uint32> m_recycleEntities;
	bool m_changeSpawnRate = true;

	Texture* m_ennemyTexture;
	Mesh* m_mesh;
};

