#include "Plane.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
	return texture0.Sample(sampler0, pin.texcoord) * pin.color;
}

