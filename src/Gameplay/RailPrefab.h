#pragma once
#include "Prefab.h"
#include <vector> 


class Texture;
class Mesh;

class RailPrefab : public Prefab
{
public:
	RailPrefab(Mesh* _box, Mesh* _cylinder, Texture* _texture);
	~RailPrefab();

private:
	std::vector<uint32> m_childEntities;
};