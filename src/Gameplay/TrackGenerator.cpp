#include "pch.h"
#include "TrackGenerator.h"
#include "RailPrefab.h"
#include "Manager/GameManager.h"
#include <random>
#include <cmath> 
#include "Mesh.h"
#include "Geometry.h"
#include "IRenderer.h"
#include "Texture.h"

TrackGenerator::TrackGenerator() : m_currentPos(0, 0, 0), m_yaw(0.0f), m_pitch(0.0f), m_segmentLength(0.1f), m_segmentsSinceLastTurn(0)
{
    m_renderer = GameManager::GInstance()->GRenderEngine();

    Geometry m_geometry2;
    m_geometry2.BuildBox(1.f, 0.5f, 1.f);
    m_box = new Mesh();
    m_renderer->InitMesh(*m_box, m_geometry2);
    m_texture = m_renderer->LoadTexture(L"../../res/Render/rail.dds");

    Geometry m_geometry;
    m_geometry.BuildCylinder(16, 1);
    m_cyl = new Mesh();
    m_renderer->InitMesh(*m_cyl, m_geometry);
}

void TrackGenerator::Generate(int _segmentCount)
{
    for (int i = 0; i < _segmentCount; ++i)
    {
        SpawnSegment();
    }
}

void TrackGenerator::SpawnSegment()
{
    RailPrefab* rail = new RailPrefab(m_box, m_cyl, m_texture);

    rail->transform->localTransform.position = m_currentPos;
    rail->transform->localTransform.SYPR(m_yaw, 0.0f, 0.0f);

    m_segmentsSinceLastTurn++;
    if (m_segmentsSinceLastTurn >= 5)
    {
        float randomTurn = ((rand() % 3) - 1) * 0.05f;
        m_yaw += randomTurn;

        // Force de rappel pour rester autour de 0 globalement
        m_yaw -= m_currentPos.x * 0.5f;

        if (m_yaw > 0.17f) m_yaw = 0.17f;
        if (m_yaw < -0.17f) m_yaw = -0.17f;

        m_segmentsSinceLastTurn = 0;
    }

    m_currentPos.x += sinf(m_yaw) * m_segmentLength;
    m_currentPos.z += cosf(m_yaw) * m_segmentLength;

    m_rails.push_back(rail);
}

void TrackGenerator::MoveRails(float speed, float deltaTime)
{
    float moveAmount = speed * deltaTime;

    // 1. On avance la piste vers le joueur sur l'axe Z
    for (RailPrefab* rail : m_rails)
    {
        rail->transform->localTransform.position.z -= moveAmount;
    }
    m_currentPos.z -= moveAmount;

    // 2. SUIVI AUTOMATIQUE DU RAIL (Le joueur reste à X=0)
    RailPrefab* railBefore = nullptr;
    RailPrefab* railAfter = nullptr;

    // On cherche les deux rails qui entourent le joueur (Z = 0)
    for (RailPrefab* rail : m_rails)
    {
        float z = rail->transform->localTransform.position.z;
        if (z >= 0.0f && (!railAfter || z < railAfter->transform->localTransform.position.z)) {
            railAfter = rail;
        }
        if (z < 0.0f && (!railBefore || z > railBefore->transform->localTransform.position.z)) {
            railBefore = rail;
        }
    }

    float offsetX = 0.0f;
    if (railBefore && railAfter)
    {
        // Calcul ultra-précis (Interpolation linéaire) pour que ça ne saccade pas
        float zDist = railAfter->transform->localTransform.position.z - railBefore->transform->localTransform.position.z;
        if (zDist > 0.001f) {
            float t = (0.0f - railBefore->transform->localTransform.position.z) / zDist;
            offsetX = railBefore->transform->localTransform.position.x + t * (railAfter->transform->localTransform.position.x - railBefore->transform->localTransform.position.x);
        }
    }
    else if (railAfter) offsetX = railAfter->transform->localTransform.position.x;
    else if (railBefore) offsetX = railBefore->transform->localTransform.position.x;

    // On décale toute la piste sur X pour ramener ce point exactement à 0
    for (RailPrefab* rail : m_rails)
    {
        rail->transform->localTransform.position.x -= offsetX;
    }
    m_currentPos.x -= offsetX;
}

void TrackGenerator::RecycleSegment()
{
    for (RailPrefab* rail : m_rails)
    {
        if (rail->transform->localTransform.position.z < -1.0f)
        {
            m_segmentsSinceLastTurn++;
            if (m_segmentsSinceLastTurn >= 5)
            {
                float randomTurn = ((rand() % 3) - 1) * 0.05f;
                m_yaw += randomTurn;
                m_yaw -= m_currentPos.x * 0.5f;

                if (m_yaw > 0.17f) m_yaw = 0.17f;
                if (m_yaw < -0.17f) m_yaw = -0.17f;

                m_segmentsSinceLastTurn = 0;
            }

            rail->transform->localTransform.position = m_currentPos;
            rail->transform->localTransform.SYPR(m_yaw, 0.0f, 0.0f);

            m_currentPos.x += sinf(m_yaw) * m_segmentLength;
            m_currentPos.z += cosf(m_yaw) * m_segmentLength;
        }
    }
}

void TrackGenerator::UpdateWorld(float speed, float deltaTime)
{
    MoveRails(speed, deltaTime);
    RecycleSegment();
}

TrackGenerator::~TrackGenerator() {}