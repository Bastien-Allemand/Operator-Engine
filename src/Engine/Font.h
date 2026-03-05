#pragma once
#include "Component/TextComponent.h"

struct charDefinition
{
	float u;
	float v;
	float width;
	float height;
	float aspectRatio;
	int32 advanceX;
};

class Font
{
public:
	void EditCharacter(TextComponent* _textComponent, char _character, float _u, float _v, float _width, float _height, int32 _advanceX);
	bool LoadFontData(TextComponent* _textComponent, WString _dataPath);
	UnOrderedMap<int, charDefinition>& GCAlphabet() { return m_alphabet; }
	float GCTexWidth() const { return m_texWidth; }
	float GCTexHeight() const { return m_texHeight; }
	float GCLineHeight() const { return m_lineHeight; }
private:
	UnOrderedMap<int, charDefinition> m_alphabet;
	float m_texWidth = 256.0f;
	float m_texHeight = 256.0f;
	float m_lineHeight = 32.0f;
};