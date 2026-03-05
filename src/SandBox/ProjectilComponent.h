#pragma once
class Geometry;
struct ProjectilComponent
{
public:
	bool toSpawn = false;
	Vector3f direction = {1,0,0};
	float lifetime = 0.0f;
};