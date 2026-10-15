#include "GeometryDebug.hlsli"

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL, float4 color : COLOR)
{
    VS_OUT vout;
	//vout.position = position;
    vout.position = mul(float4(position), cameraViewProjection);
    vout.color = color;

    return vout;
}
