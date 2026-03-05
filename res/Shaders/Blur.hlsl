#include "PostProcessCommon.hlsli"

[numthreads(16, 16, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x >= (uint) gResolution.x || dispatchThreadID.y >= (uint) gResolution.y)
        return;

    int2 pos = int2(dispatchThreadID.xy);

    int blurRadius = (int) max(0.0, gParams0.x);
    
    // Notre "interrupteur" de direction (0 = Horizontal, 1 = Vertical)
    bool isVertical = (gParams0.z > 0.5f);
    
    if (blurRadius == 0)
    {
        gOutput[pos] = gInput.Load(int3(pos, 0));
        return;
    }

    float4 resultColor = float4(0, 0, 0, 0);
    float totalWeight = 0.0;
    
    for (int i = -blurRadius; i <= blurRadius; ++i)
    {

        int2 offset = isVertical ? int2(0, i) : int2(i, 0);
        int2 samplePos = pos + offset;
        samplePos.x = clamp(samplePos.x, 0, (int) gResolution.x - 1);
        samplePos.y = clamp(samplePos.y, 0, (int) gResolution.y - 1);

        resultColor += gInput.Load(int3(samplePos, 0));
        totalWeight += 1.0;
    }
    
    gOutput[pos] = resultColor / totalWeight;
}