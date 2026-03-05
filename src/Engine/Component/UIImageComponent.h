#pragma once
struct Texture;

struct UIImageComponent
{
public:
	Vector4f color = Vector4f(1.f, 1.f, 1.f, 1.f);
	
	Texture* texture = nullptr;

	bool initialized = false;
	bool visible = true;

	const wchar_t* texturePath = nullptr;

	void InitImage();
};