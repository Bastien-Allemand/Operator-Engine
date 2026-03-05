#pragma once

struct Light;
struct LightComponent
{
public:
	enum LightType
	{
		Ambient = 0,
		Directional,
		Point,
		Spot
	} 
	Type = LightType::Ambient;

	Light* LightData = nullptr;

	bool initialize = false;
};

