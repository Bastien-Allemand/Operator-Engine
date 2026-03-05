#pragma once
#include "Prefab.h"
class Light;

class Font; 


class Mesh;
class RailPrefab;
class IRenderer;

class PlayerPrefab : public Prefab
{
public:

	PlayerPrefab(Mesh* _box);
	~PlayerPrefab();
private:
	Font* m_font;

	Light* m_light;

};




