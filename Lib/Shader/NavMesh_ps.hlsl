#include "NavMesh.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
    return float4(0, 0, 1, 0.2f);
    return pin.color;
}
