#include "pch.h"
#include "UIImageComponent.h"
#include "Manager/GameManager.h"
#include "IRenderer.h"

void UIImageComponent::InitImage()
{
	if (texture != nullptr)
	{
		initialized = true;
		return;
	}

	if (texturePath == nullptr)
	{
		Log(false, "Texture path is null");
		return;
	}

	GameManager* gameManager = GameManager::GInstance();
	IRenderer* renderEngine = gameManager->GRenderEngine();

	if (renderEngine) {

		if (texture == nullptr) {
			texture = renderEngine->LoadTexture(texturePath);
		}

		initialized = true;
	}
	Log(false, "Image initialized successfully");
}
