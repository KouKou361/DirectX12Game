#include "BasicShader.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

