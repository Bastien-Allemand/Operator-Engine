#pragma once

class Camera
{
public:
	Camera() = default;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	float fov = 0.25f * DirectX::XM_PI;
};