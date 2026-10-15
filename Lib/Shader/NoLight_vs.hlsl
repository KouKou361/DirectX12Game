
#include "NoLight.hlsli"

PS_IN main(VS_IN input)
{
#if 1
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        p += (input.boneWeights[i] * mul(input.position, boneTransforms[input.boneIndices[i]])).xyz;
        n += (input.boneWeights[i] * mul(float4(input.normal.xyz, 0), boneTransforms[input.boneIndices[i]])).xyz;
    }
#else
	float3 p = position.xyz;
	float3 n = normal;
#endif
    PS_IN vout;
    vout.position = mul(float4(p, 1.0f), cameraViewProjection);
    vout.color = input.color;
    vout.texcoord = input.texcoord;

    return vout;
}
