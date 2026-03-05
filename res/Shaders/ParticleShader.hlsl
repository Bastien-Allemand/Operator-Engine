#include "ParticleCommon.hlsli"

cbuffer cbPass : register(b0)
{
    float4x4 gviewProj;
    float3   gEyePosW;
    float    cbPassPad;
};

StructuredBuffer<Particle> gParticlesSRV : register(t0);
Texture2D    gDiffuseMap : register(t1);
SamplerState gSamLinear  : register(s0);

// Quad local : 4 coins en triangle strip
static const float2 gQuadOffsets[4] =
{
    float2(-0.5f, -0.5f),
    float2( 0.5f, -0.5f),
    float2(-0.5f,  0.5f),
    float2( 0.5f,  0.5f)
};

static const float2 gQuadUVs[4] =
{
    float2(0, 1),
    float2(1, 1),
    float2(0, 0),
    float2(1, 0)
};

struct VertexOut
{
    float4 posH  : SV_POSITION;
    float2 vT    : TEXCOORD;
    float4 color : COLOR;
};

VertexOut VS(uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    VertexOut vOut = (VertexOut)0;

    Particle p = gParticlesSRV[instanceID];
    
    if (p.age >= p.lifeTime || p.lifeTime <= 0.0f)
    {
        vOut.posH = float4(0, 0, -1, 1);
        vOut.color = float4(0, 0, 0, 0);
        return vOut;
    }
    
    float3 toEye = normalize(gEyePosW - p.position);
    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, toEye));
    up = cross(toEye, right);

    float2 offset = gQuadOffsets[vertexID] * p.size;
    float3 worldPos = p.position + right * offset.x + up * offset.y;

    vOut.posH = mul(float4(worldPos, 1.0f), gviewProj);
    vOut.vT = gQuadUVs[vertexID];
    vOut.color = p.color;

    return vOut;
}

float4 PS(VertexOut pIn) : SV_Target
{
    float4 texColor = gDiffuseMap.Sample(gSamLinear, pIn.vT);
    float4 finalColor = texColor * pIn.color;
    return finalColor;
}