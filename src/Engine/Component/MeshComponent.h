#pragma once
#include "Geometry.h"

class Mesh;
struct Texture;

struct MeshComponent
{
public:
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
	bool visible = true;
};