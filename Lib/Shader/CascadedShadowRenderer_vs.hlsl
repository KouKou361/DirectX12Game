                                           
#include "CascadedShadowRenderer.hlsli"

ShadowMapPS_IN main(VS_IN input,uint instNo:SV_InstanceID)
{
#if 1
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    
 
    
    for (int i = 0; i < 4; i++)
    {
        if (instNo==0)
        {
            p += (input.boneWeights[i] * mul(input.position, boneTransforms[input.boneIndices[i]])).xyz;
            n += (input.boneWeights[i] * mul(float4(input.normal.xyz, 0), boneTransforms[input.boneIndices[i]])).xyz;
        }
        else
        {
            p += (input.boneWeights[i] * mul(input.position, boneTransforms[instNo])).xyz;
            n += (input.boneWeights[i] * mul(float4(input.normal.xyz, 0), boneTransforms[instNo])).xyz;
        }
            
    }
#else
    float3 p = input.position.xyz;
    float3 n = input.normal;
#endif
    ShadowMapPS_IN vout;
    vout.position = mul(float4(p, 1.0f), cameraViewProjection);
    vout.color = input.color;
    vout.texcoord = input.texcoord;
    vout.Wposition = p;
    vout.normal = normalize(n);

    return vout;
}
