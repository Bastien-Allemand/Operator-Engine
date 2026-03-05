#pragma once
#include "ISystem.h"
class Mesh;
struct Texture;	

class ProjectilSystem : public ISystem
{
public:
	ProjectilSystem();
	void Init(GameManager* _gameManager) override;
	void Update(GameManager* _gameManager) override;
private:
	Mesh* m_projectilMesh;
	Texture* m_projectilTexture;
	bool m_isInit = false;
};

