#pragma once
#include "Transform.h"

struct TransformComponent
{
public:
	Transform localTransform;

	TransformComponent* parent = nullptr;
	Vector<TransformComponent*> children;

	Transform BuildworldTransfrom() const;
	Matrix BuildWorldMatrix() const;
};