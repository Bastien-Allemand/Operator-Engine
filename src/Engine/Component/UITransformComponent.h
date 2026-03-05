#pragma once

struct UITransformComponent
{
public:
	Vector2f anchorMin = Vector2f(0.5f, 0.5f);
	Vector2f anchorMax = Vector2f(0.5f, 0.5f);

	Vector2f anchoredPosition = Vector2f(0.0f, 0.0f);
	Vector2f size = Vector2f(100.0f, 100.0f);

	Vector2f pivot = Vector2f(0.5f, 0.5f);

	Vector2f screenPosition;
	Vector2f screenSize;

	int zIndex = 0;
	bool isActive = true;
};