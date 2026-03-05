#include "pch.h"
#include "Prefab.h"
#include "Tag.h"


Prefab::Prefab()
{
	gameManager = GameManager::GInstance();
	componentManager = gameManager->GComponentManager();
	id = gameManager->GNewEntityId();
}

Prefab::~Prefab() {}