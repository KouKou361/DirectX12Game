#include "Lambert.hlsli"

float4 main(PS_IN pin) : SV_TARGET
{
	return texture0.Sample(sampler0, pin.texcoord) * pin.color * materialColor;
}

