#pragma once
class GameManager;
class Mesh;
struct Light;
struct Texture;
class App
{
public:
	void Initialize(GameManager* _gameManager);
	~App();
	void Run();
private:
	Mesh* m_mesh;
	Mesh* m_mesh2;
	Mesh* m_mesh3;
	Mesh* m_mesh4;
	Texture* m_texture;

	Light* m_light;
	Light* m_light2;
};

