#pragma once
#include "IPostProcessSystemRessource.h"

struct PostProcessComponent
{
	IPostProcessSystemRessource* postProcessResource = nullptr;

	Vector4f params0;
	Vector4f params1;
	
	WString* shaderPath;

	uint32 zOrder = 0;
	
	bool isActive = false;

	~PostProcessComponent()
	{
		if (postProcessResource != nullptr)
			postProcessResource->Release();
	}
};