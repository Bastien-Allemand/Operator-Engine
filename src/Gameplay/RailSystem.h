#pragma once
#include "ISystem.h"
#include "TrackGenerator.h"

class RailSystem : public ISystem
{
public:
    RailSystem();
    void Init(GameManager* _gameManager) override;
    void Update(GameManager* _gameManager) override;

private:
    TrackGenerator m_trackGenerator;
    float m_scrollSpeed = 1.0f;
};