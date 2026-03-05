#include "lighting.hlsl"

cbuffer cbPerObject : register(b0)
{
    float4x4 gworld;
    float4x4 gworldInvTrans;
    float4 gUvRect;
    float4 gColor;
};

Texture2D gDiffuseMap : register(t0);
SamplerState gSamLinear : register(s0);

cbuffer cbPass : register(b1)
{
    float4x4 gviewProj; 
    float3 gEyePosW; 
    float cbPassPad; 
    Light gAmbientLight; 
    Light gDirectLight; 
    Light gPointLight; 
    Light gSpotLight; 
};

struct VertexIn
{
    float3 v : POSITION;
    float3 vN : NORMAL;
    float2 vT : TEXCOORD;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
    float3 vW : POSITION; 
    float3 vN : NORMAL;
    float2 vT : TEXCOORD;
    float4 color : COLOR;
};

VertexOut VS(VertexIn vIn)
{
    VertexOut vOut = (VertexOut) 0.0f;

    // 1. Transformation vers l'espace de projection (pour l'écran)
    float4x4 worldViewProj = mul(gworld, gviewProj);
    vOut.posH = mul(float4(vIn.v, 1.0f), worldViewProj);

    // 2. CORRECTION : Calcul de la position dans le Monde (utile pour plus tard)
    vOut.vW = mul(float4(vIn.v, 1.0f), gworld).xyz;

    // 3. CORRECTION : Transformation de la normale dans l'espace Monde
    // On utilise uniquement gworldInvTrans et on le convertit en 3x3
    vOut.vN = mul(vIn.vN, (float3x3) transpose(gworldInvTrans));

    // 3. Passage des autres données
    vOut.vT = gUvRect.xy + vIn.vT * gUvRect.zw;
    vOut.color = vIn.color;
    
    return vOut;
}

float4 PS(VertexOut pIn) : SV_Target
{
    float4 texColor = gDiffuseMap.Sample(gSamLinear, pIn.vT);
    float4 baseColor = texColor * pIn.color * gColor;

    float3 N = normalize(pIn.vN);

    float3 totalLight = gAmbientLight.Strength + ComputeDiffuse(gDirectLight, N) + ComputePointLight(gPointLight, pIn.vW, N) + ComputeSpotLight(gSpotLight, pIn.vW, N); 

    float4 finalColor = float4(totalLight, 1.0f) * baseColor;
    finalColor.a = baseColor.a;
    
    return finalColor;
}