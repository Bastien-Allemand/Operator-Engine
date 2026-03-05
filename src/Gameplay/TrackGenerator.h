#pragma once
#include <vector>

class Mesh;
class RailPrefab;
class IRenderer;
class Texture;

class TrackGenerator
{
public:
    TrackGenerator();
    ~TrackGenerator();

    void Generate(int _segmentCount);
    void MoveRails(float speed, float deltaTime);
    void RecycleSegment();
    void UpdateWorld(float speed, float deltaTime);

private:
    void SpawnSegment();

    IRenderer* m_renderer;
    Mesh* m_box;
    Mesh* m_cyl;
    Texture* m_texture;

    Vector3f m_currentPos;
    float m_yaw;
    float m_pitch;

    float m_segmentLength;
    int m_segmentsSinceLastTurn;

    std::vector<RailPrefab*> m_rails;
};