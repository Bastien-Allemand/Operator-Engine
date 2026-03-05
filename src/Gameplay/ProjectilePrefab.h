#pragma once
#include "Prefab.h"

class Mesh;
class RailPrefab;
class IRenderer;
class Texture;
enum ProjectileType;

class ProjectilePrefab : public Prefab
{
public:
	ProjectilePrefab(Mesh* _mesh, Texture* _texture, ProjectileType _type);
	~ProjectilePrefab();
};

