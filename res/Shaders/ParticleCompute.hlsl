#include "ParticleCommon.hlsli"

cbuffer CbParticle : register(b0)
{
    float3 emitterPosition;
    float  dt;
    
    uint   numToSpawn;
    uint   particleType;
    uint   randomSeed;
    float pad0;
    
    float3 minVelocity;
    float  minLifetime;
    
    float3 maxVelocity;
    float  maxLifetime;
    
    float4 startColor;
    
    float4 endColor;
    
    float  startSize;
    float  endSize;
    int shapeType;
    float pad1;
    
    float shapeRadius;
    float3 shapeBoxExtents;
    
    float3 direction;
    float force;
};

RWStructuredBuffer<Particle> gParticles : register(u0);
ConsumeStructuredBuffer<uint> gFreeList : register(u1);
AppendStructuredBuffer<uint> gDeadList : register(u1);

float Random01(inout uint state)
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return float(state) / 4294967296.0f;
}

[numthreads(64, 1, 1)]
void CSUpdate(uint3 dtid : SV_DispatchThreadID)
{
    uint id = dtid.x;
    Particle p = gParticles[id];

    if (p.lifeTime <= 0.0f) return;

    p.age += dt;

    if (p.age >= p.lifeTime)
    {
        //// IncrementCounter retourne la valeur AVANT incrément = position d'écriture
        //uint slot = gFreeList.IncrementCounter();
        //gFreeList[slot] = id;
        gDeadList.Append(id);
        p.lifeTime = -1.0f; 
        gParticles[id] = p;
        return;
    }

    float t = p.age / p.lifeTime;       
    
    p.position += p.velocity * dt;
    p.velocity += direction * force * dt;
    p.color = lerp(startColor, endColor, t);
    p.size = lerp(startSize, endSize, t);

    gParticles[id] = p;
}

[numthreads(1, 1, 1)]
void CSEmit(uint3 dtid : SV_DispatchThreadID)
{
    uint id = dtid.x;
    if (id >= numToSpawn) return;        
    
    uint freeIndex = gFreeList.Consume();

    uint rngState = randomSeed + freeIndex + id;

    Particle p;

    float3 localPos = float3(0, 0, 0);

    switch (shapeType)
    {
        case 1:
        {
            float theta = Random01(rngState) * 2.0f * 3.14159f;
            float phi = acos(2.0f * Random01(rngState) - 1.0f);
            float r = shapeRadius * pow(Random01(rngState), 1.0f / 3.0f);
            localPos = float3(r * sin(phi) * cos(theta), r * sin(phi) * sin(theta), r * cos(phi));
            break;
        }
        case 2:
        {
            localPos = float3(
                (Random01(rngState) * 2.0f - 1.0f) * shapeBoxExtents.x,
                (Random01(rngState) * 2.0f - 1.0f) * shapeBoxExtents.y,
                (Random01(rngState) * 2.0f - 1.0f) * shapeBoxExtents.z
            );
            break;
        }
        case 3:
        {
                float h = shapeBoxExtents.y * frac(Random01(rngState));
                float maxR = h * tan(45);
                float r = maxR * sqrt(frac(Random01(rngState)));
                float theta = frac(Random01(rngState)) * 2.0f * 3.14159f;
                localPos = float3(r * cos(theta), -h + 1, r * sin(theta));
                break;
            }
        case 0:
            break;
        default:
            break;
    }
    p.position = emitterPosition + localPos;
    p.lifeTime = lerp(minLifetime, maxLifetime, Random01(rngState));
    
    p.velocity = float3(
        lerp(minVelocity.x, maxVelocity.x, Random01(rngState)),
        lerp(minVelocity.y, maxVelocity.y, Random01(rngState)),
        lerp(minVelocity.z, maxVelocity.z, Random01(rngState))
    );

    p.size = startSize;
    p.color = startColor;
    p.type = particleType;
    p.rotation = 0;
    p.age = 0;
    p.custom = 0;

    gParticles[freeIndex] = p;
}