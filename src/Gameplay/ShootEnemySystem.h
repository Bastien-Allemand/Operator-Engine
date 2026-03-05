#pragma once
#include "ISystem.h"

class Mesh;
class Texture;

class ShootEnemySystem : public ISystem
{
	public:
	ShootEnemySystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
private:
	Texture* m_projectileTexture;
	Mesh* m_mesh;
};

