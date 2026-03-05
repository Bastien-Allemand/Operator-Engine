#include "pch.h"
#include "RailSystem.h"
#include "Manager/GameManager.h"

RailSystem::RailSystem()
{
}

void RailSystem::Init(GameManager* _gameManager)
{
    m_trackGenerator.Generate(100);
}

void RailSystem::Update(GameManager* _gameManager)
{
    m_trackGenerator.UpdateWorld(m_scrollSpeed, _gameManager->GCDeltaTime());
}