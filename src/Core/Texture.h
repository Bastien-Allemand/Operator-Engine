#pragma once

struct Image;

struct Texture
{
	Image* image = nullptr;
	Vector4f uvOffset = { 0,0,1,1 };
};

