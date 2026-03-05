#include "pch.h"
#include "UISystem.h"
#include "Manager/GameManager.h"
#include "Manager/ComponentManager.h"
#include "ComponentPool.h"
#include "Component/UITransformComponent.h"
#include "Component/UIImageComponent.h"
#include "Component/TextComponent.h"
#include "IRenderer.h"
#include "Core/Window.h"
#include "Font.h"

UISystem::UISystem()
{
	m_hashCode = typeid(UITransformComponent).hash_code();
}

void UISystem::Init(GameManager* _gameManager)
{
	m_renderer = _gameManager->GRenderEngine();
	m_componentManager = _gameManager->GComponentManager();
}

void UISystem::Update(GameManager* _gameManager)
{
	m_uiPool = _gameManager->GComponentManager()->GPool<UITransformComponent>();
	ComputeScreenRect(_gameManager);

	ComponentPool<UIImageComponent>* imgPool = _gameManager->GComponentManager()->GPool<UIImageComponent>();
	ComponentPool< TextComponent>* textPool = _gameManager->GComponentManager()->GPool<TextComponent>();

	std::stable_sort(m_entities.begin(), m_entities.end(), [&](uint32 a, uint32 b) {
		return m_uiPool->GComponent(a)->zIndex < m_uiPool->GComponent(b)->zIndex;
		});

	m_renderer->BeginDrawUI();

	for (uint32 entity : m_entities)
	{
		UITransformComponent* ui = m_uiPool->GComponent(entity);

		if (ui == nullptr || !ui->isActive) continue;

		UIImageComponent* img = imgPool->GComponent(entity);

		if (img && !img->visible) continue;
		if (img && !img->initialized) img->InitImage();

		TextComponent* text = textPool->GComponent(entity);

		if (text && !text->isInitialized)
		{
			if (text->pathTexture)
			{
				text->texture = m_renderer->LoadTexture(text->pathTexture->c_str());
			}
			if (text->pathData)
			{
				Font* font = new Font();
				font->LoadFontData(text, *text->pathData);
			}
			text->isInitialized = true;
		}

		if (img)
		{
			Texture* texture = img->texture;
			Vector4f color = img->color;
			m_renderer->DrawUI(texture, ui->screenPosition, ui->screenSize, color);
		}

		if (text && text->font)
		{
			float fontSize = ui->screenSize.y;
			float scale = fontSize / text->font->GCLineHeight();
			Vector2f charPos = ui->screenPosition;

			for (wchar_t c : text->text)
			{
				auto& alphabet = text->font->GCAlphabet();

				if (c == L' ')
				{
					if (alphabet.find(c) != alphabet.end())
						charPos.x += alphabet[c].advanceX * scale;
					else
						charPos.x += fontSize * 0.5f;
					continue;
				}

				if (alphabet.find(c) == alphabet.end())
				{
					Log(true, "Character not found in font");
					continue;
				}

				charDefinition def = alphabet[c];

				float charH = fontSize;
				float charW = def.aspectRatio * charH;
				Vector2f charSize = { charW, charH };

				Vector4f uvRect = { def.u, def.v, def.width, def.height };

				m_renderer->DrawUI(text->texture, charPos, charSize, Vector4f(1.f, 1.f, 1.f, 1.f), uvRect);
				charPos.x += def.advanceX * scale;
			}
		}
	}
}

void UISystem::ComputeScreenRect(GameManager* _gameManager)
{
	float sw = (float)_gameManager->GWindow()->GCWidth();
	float sh = (float)_gameManager->GWindow()->GCHeight();

	for (uint32 entity : m_uiPool->GEntityIds())
	{
		UITransformComponent* c = m_uiPool->GComponent(entity);
		if (!c || !c->isActive) continue;

		c->screenSize.x = c->size.x + (c->anchorMax.x - c->anchorMin.x) * sw;
		c->screenSize.y = c->size.y + (c->anchorMax.y - c->anchorMin.y) * sh;

		c->screenPosition.x = c->anchoredPosition.x + (c->anchorMin.x * sw);
		c->screenPosition.y = c->anchoredPosition.y + (c->anchorMin.y * sh);

		c->screenPosition.x -= c->pivot.x * c->screenSize.x;
		c->screenPosition.y -= c->pivot.y * c->screenSize.y;
	}
}