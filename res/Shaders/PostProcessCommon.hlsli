cbuffer cbPostProcess : register(b0)
{
    float2 gResolution;
    float2 gInvResolution;
    float gTime;
    float gDeltaTime;
    float2 gPad;
    
    float4 gParams0;
    float4 gParams1;
};

Texture2D gInput : register(t0);
RWTexture2D<float4> gOutput : register(u0);
SamplerState gSamLinear : register(s0);