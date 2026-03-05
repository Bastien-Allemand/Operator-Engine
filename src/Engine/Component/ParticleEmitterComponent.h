#pragma once
struct Texture;

enum class EmitterShape
{
    POINT,
	SPHERE,
	BOX,
	CONE
	// Add more shapes as needed
	// For example:
	// 3D models, custom meshes, etc.
};

struct ParticleEmitterComponent
{
public:
    float spawnRate = 100.0f;
    float accumulator = 0.0f;
    uint32_t maxParticles = 10000;

    float minLifetime = 1.0f;
    float maxLifetime = 2.0f;

    Vector3f minVelocity = { .0f, .0f, .0f };
    Vector3f maxVelocity = { .0f, .0f, .0f };

    Vector4f startColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4f endColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float startSize = 0.5f;
    float endSize = 0.1f;

    uint32_t particleType = 0;
    EmitterShape shape = EmitterShape::SPHERE;
    float shapeRadius = 1.0f;
    Vector3f shapeBoxExtents = { 1.0f, 1.0f, 1.0f };

	Vector3f direction = { 0.0f, 0.0f, 0.0f };
	float force = 0.0f;

    Texture* m_texture = nullptr;
    WString* m_pathTexture = nullptr;
};

