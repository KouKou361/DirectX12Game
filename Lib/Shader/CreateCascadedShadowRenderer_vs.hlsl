#include "CreateCascadedShadowRenderer.hlsli"

PS_ShadowIN main(VS_IN input, uint instNo : SV_InstanceID)
{
#if 1
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
   // for (int i = 0; i < 4; i++)
   // {
   //     p += (input.boneWeights[i] * mul(input.position, boneTransforms[input.boneIndices[i]])).xyz;
   //     
   //     //影生成の為、色は必要なしと判断。
   //     //n += (input.boneWeights[i] * mul(float4(input.normal.xyz, 0), boneTransforms[input.boneIndices[i]])).xyz;
   // }
    
    for (int i = 0; i < 4; i++)
    {
        if (instNo == 0)
        {
            p += (input.boneWeights[i] * mul(input.position, boneTransforms[input.boneIndices[i]])).xyz;
        }
        else
        {
            p += (input.boneWeights[i] * mul(input.position, boneTransforms[instNo])).xyz;
        }
            
    }
    
#else
	float3 p = position.xyz;
	float3 n = normal;
#endif
    PS_ShadowIN vout;
    vout.position = mul(float4(p, 1.0f), createCascadeShadowLightViewProjection[shodowMapIndex]);
    vout.Depth = vout.position;

    return vout;
}
