#pragma once

class Font;

struct Texture;

struct fontVertex
{
	Vector3f position;
	Vector2f uv;
};

struct TextComponent
{
public:
	Texture* texture = nullptr;
	WString* pathTexture = nullptr;
	WString* pathData = nullptr;

	Font* font = nullptr;

	int32 rows = 0;
	int32 cols = 0;
	float charWidth = 0.0f;
	float charHeight = 0.0f;

	Vector<fontVertex> vertices;
	Vector<uint32> indices;

	WString text;

	bool isInitialized = false;
};