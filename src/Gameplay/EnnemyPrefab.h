#pragma once
#include "Prefab.h"

class Mesh;
class RailPrefab;
class IRenderer;
class Texture;

class EnnemyPrefab: public Prefab
{
public:
	EnnemyPrefab(Mesh* _mesh, Texture* _texture);
	~EnnemyPrefab();

};

