#pragma once
class Camera;

struct CameraComponent
{
public:
	CameraComponent();
	bool mainCamera = false;
	bool swapTothisCamera = false;
	Camera* camera;
};

