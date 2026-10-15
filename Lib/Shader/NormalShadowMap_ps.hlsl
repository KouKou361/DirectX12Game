#include "NormalShadowMap.hlsli"

float4 main(ShadowMapPS_IN pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord) * pin.color * materialColor;
    float3 posFromLightVP = pin.vShadow.xyz / pin.vShadow.w;
    float2 shadowUV = (posFromLightVP + float2(1, -1)) * float2(0.5f, -0.5f);
    
    float depthFromLight = ShadowTexture0.SampleCmp(shadowSmp
    , shadowUV, posFromLightVP.z-0.00005f);
  
    
    float shadowWeight = 1.0f;

    if (depthFromLight<posFromLightVP.z-Bias)
    {
        shadowWeight = lerp(0.5f, 1.0f, depthFromLight);
    }
 //   float4 color = texture0.Sample(sampler0, pin.texcoord) * pin.color * materialColor;
 //   	// シャドウマップ適用
 //   color.rgb *= GetShadow(shadowMapTexture, sampler0,
 //   		GetShadowTex(shadowViewProjection, pin.vShadow), float3(0.5, 0.5, 0.5), Bias);
 //   
 // //return  texture0.Sample(shadowMapTexture, pin.vShadow) * pin.color * materialColor;
    color.xyz *= shadowWeight;
    return color;
}

