#include "PostProcessCommon.hlsli"

float GetLuma(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

[numthreads(16, 16, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x >= (uint) gResolution.x || dispatchThreadID.y >= (uint) gResolution.y)
        return;

    int2 pos = int2(dispatchThreadID.xy);
    float4 color = gInput.Load(int3(pos, 0));
    float luma = GetLuma(color.rgb);
    
    float levels = max(1.0, gParams0.x);
    float edgeThreshold = gParams0.y;
    float3 edgeColor = float3(gParams0.z, gParams0.w, gParams1.x);
    float edgeMultiplier = gParams1.y;
    float blendEffect = saturate(gParams1.z);

    float toonLuma = floor(luma * levels) / levels;
    float3 celColor = color.rgb * (toonLuma / max(luma, 0.001));

    float s11 = GetLuma(gInput.Load(int3(pos + int2(-1, -1), 0)).rgb);
    float s12 = GetLuma(gInput.Load(int3(pos + int2(0, -1), 0)).rgb);
    float s13 = GetLuma(gInput.Load(int3(pos + int2(1, -1), 0)).rgb);
    
    float s21 = GetLuma(gInput.Load(int3(pos + int2(-1, 0), 0)).rgb);
    // s22 est notre pixel actuel (luma)
    float s23 = GetLuma(gInput.Load(int3(pos + int2(1, 0), 0)).rgb);
    
    float s31 = GetLuma(gInput.Load(int3(pos + int2(-1, 1), 0)).rgb);
    float s32 = GetLuma(gInput.Load(int3(pos + int2(0, 1), 0)).rgb);
    float s33 = GetLuma(gInput.Load(int3(pos + int2(1, 1), 0)).rgb);
    
    float gx = s13 + (2.0 * s23) + s33 - s11 - (2.0 * s21) - s31;
    float gy = s31 + (2.0 * s32) + s33 - s11 - (2.0 * s12) - s13;
    float gradient = sqrt(gx * gx + gy * gy) * edgeMultiplier;
    
    float edge = step(edgeThreshold, gradient);
    float3 finalRGB = lerp(celColor, edgeColor, edge);
    
    finalRGB = lerp(color.rgb, finalRGB, blendEffect);
    
    gOutput[pos] = float4(finalRGB, color.a);
}