#include "PostProcessCommon.hlsli"

[numthreads(16, 16, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float2 uv = dispatchThreadID.xy * gInvResolution;
    float4 color = gInput.Load(int3(dispatchThreadID.xy, 0));
    
    float saturation = gParams0.x;
    float3 gray = dot(color.rgb, float3(0.299, 0.587, 0.114));
    color = lerp(float4(gray, color.a), color, saturation);
    
    gOutput[dispatchThreadID.xy] = color;
}