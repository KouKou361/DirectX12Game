#include "NavMesh.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR)
{
    VS_OUT vout;
	//vout.position = position;
    position.y += 1.0f;
    vout.position = mul(float4(position), world);
    vout.position = mul(float4(vout.position), cameraViewProjection);
    vout.color = color;

    return vout;
}
