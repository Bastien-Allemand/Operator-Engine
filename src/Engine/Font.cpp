#include "pch.h"
#include "Font.h"
#include <fstream>

void Font::EditCharacter(TextComponent* _textComponent, char _character, float _u, float _v, float _width, float _height, int32 _advanceX)
{
	charDefinition def;
	def.u = _u;
	def.v = _v;
	def.width = _width;
	def.height = _height;
	def.aspectRatio = _width / _height;
	def.advanceX = _advanceX;
	m_alphabet[_character] = def;
}

bool Font::LoadFontData(TextComponent* _textComponent, WString _dataPath)
{
	std::wifstream file(_dataPath);
	if (!file.is_open())
	{
		Log(true, "Failed to open font data file");
		return 1;
	}

	WString line;
	while (std::getline(file, line))
	{
		if (line.substr(0, 5) == L"char ")
		{
			int id, x, y, width, height, xadvance;
			if (swscanf_s(line.c_str(), L"char id=%d x=%d y=%d width=%d height=%d xoffset=%*d yoffset=%*d xadvance=%d",
				&id, &x, &y, &width, &height, &xadvance) == 6)
			{
				float u = (float)x / m_texWidth;
				float v = (float)y / m_texHeight;
				float wUV = (float)width / m_texWidth;
				float hUV = (float)height / m_texHeight;
				EditCharacter(_textComponent, id, u, v, wUV, hUV, xadvance);
			}
		}
		else if (line.substr(0, 7) == L"common ")
		{
			int lineHeight;
			swscanf_s(line.c_str(), L"common lineHeight=%d base=%*d scaleW=%f scaleH=%f", &lineHeight, &m_texWidth, &m_texHeight);
			m_lineHeight = (float)lineHeight;
		}
	}
	file.close();
	_textComponent->font = this;
	Log(false, "Font data loaded successfully");
	return 0;
}
