#include "CreateCascadedShadowRenderer.hlsli"

float4 main(PS_ShadowIN pin) : SV_TARGET
{
    float LightDepth = pin.Depth.z / pin.Depth.w;
    return float4(LightDepth, LightDepth * LightDepth, 0, 1);
}

