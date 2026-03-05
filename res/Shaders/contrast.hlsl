#include "PostProcessCommon.hlsli"

[numthreads(16, 16, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float2 uv = dispatchThreadID.xy * gInvResolution;
    float4 color = gInput.Load(int3(dispatchThreadID.xy, 0));
    
    float contrast = gParams0.x;
    color.rgb = ((color.rgb - 0.5) * contrast) + 0.5;
    
    gOutput[dispatchThreadID.xy] = color;
}