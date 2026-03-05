#pragma once

struct alignas(16) CbPerObject {
	Matrix4x4f gWorld = MATRIX_IDENTITY;
	Matrix4x4f gWorldInvTrans = MATRIX_IDENTITY;
	Vector4f gUvRect = { 0.0f, 0.0f, 1.0f, 1.0f };
	Vector4f gColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};
struct Light {
	Vector3f strength = {0,0,0};
	float  falloffStart = 0;
	Vector3f direction = {0,0,1};
	float  falloffEnd = 0;
	Vector3f position = {0,0,0};
	float  spotPower = 0;
};

struct alignas(16) CbPass
{
	Matrix4x4f gViewProj = MATRIX_IDENTITY;       // 64 bytes
	Vector3f eyePosW = VEC3_FOWARD;           // 12 bytes
	float pad = 0.0f;                  // 4 bytes
	Light ambientLight;         // 48 bytes
	Light dirLight;             // 48 bytes
	Light pointLight;           // 48 bytes
	Light spotLight;            // 48 bytes
};

struct alignas(16) CbParticle
{	
	Vector3f emitterPosition; //12 bytes
	float dt; //4 bytes

	uint32_t numToSpawn; //4 bytes
	uint32_t particleType; //4 bytes
	uint32_t randomSeed; //4 bytes
	float pad0; //4 bytes padding to align to 16 bytes

	Vector3f minVelocity; //12 bytes
	float minLifetime; //4 bytes

	Vector3f maxVelocity; //12 bytes
	float maxLifetime; //4 bytes

	Vector4f startColor; //16 bytes

	Vector4f endColor; //16 bytes

	float startSize; //4 bytes
	float endSize; //4 bytes
	int shapeType; //4 bytes
	float pad1; //4 bytes padding to align the total size to a multiple of 16 bytes

	float shapeRadius; //4 bytes
	Vector3f shapeBoxExtents; //12 bytes

	Vector3f direction; //12 bytes
	float force; //4 bytes	
};

struct alignas(16) CbPostProcess
{
	Vector2f resolution; //8 bytes
	Vector2f invResolution; //8 bytes
	float time; //4 bytes
	float deltaTime; //4 bytes
	float pad0[2]; //8 bytes padding to align to 16 bytes
	Vector4f params0; //16 bytes
	Vector4f params1; //16 bytes
};