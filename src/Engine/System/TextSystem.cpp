#include "pch.h"
#include "TextSystem.h"
#include "Component/TextComponent.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "ComponentPool.h"
#include "IRenderer.h"
#include "Font.h"

TextSystem::TextSystem()
{
	m_hashCode = typeid(TextComponent).hash_code();
}

void TextSystem::Init(GameManager* _gameManager)
{
	m_componentManager = _gameManager->GComponentManager();
	m_renderer = _gameManager->GRenderEngine();
}

void TextSystem::Update(GameManager* _gameManager)
{
	for (uint32 entityId : m_entities)
	{
		TextComponent* textComponent = m_componentManager->GComponent<TextComponent>(entityId);
		if (!textComponent->isInitialized)
		{
			// Load texture
			if (textComponent->pathTexture)
			{
				textComponent->texture = m_renderer->LoadTexture(textComponent->pathTexture->c_str());
			}
			// Load font data
			if (textComponent->pathData)
			{
				Font font;
				font.LoadFontData(textComponent, *textComponent->pathData);
			}
			textComponent->isInitialized = true;
		}
	}
}