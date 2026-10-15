                                           
#include "NormalShadowMap.hlsli"

ShadowMapPS_IN main(VS_IN input)
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
    ShadowMapPS_IN vout;
    vout.position = mul(float4(p, 1.0f), cameraViewProjection);

    float3 N = normalize(n);
    float3 L = normalize(-lightDirection.xyz);
    float d = dot(L, N);
    float power = max(0, d) * 0.5f + 0.5f;
    vout.color.rgb = input.color.rgb * power;
    vout.color.a = input.color.a;
    vout.texcoord = input.texcoord;

    vout.vShadow = mul(float4(p, 1.0f),shadowViewProjection);

    return vout;
}
