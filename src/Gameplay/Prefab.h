#pragma once
#include "Manager/GameManager.h"
#include "Component/TransformComponent.h"

#include "Tag.h"
class Prefab
{
public:
	TransformComponent* transform = nullptr;

	Prefab();
	virtual ~Prefab();

	uint64 GID() const { return id; }
protected:
	GameManager* gameManager = nullptr;
	ComponentManager* componentManager = nullptr;
	uint32 id;

};



