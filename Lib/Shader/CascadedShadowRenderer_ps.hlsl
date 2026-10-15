#include "CascadedShadowRenderer.hlsli"

float4 main(ShadowMapPS_IN pin) : SV_TARGET
{
 
    
    float3 lambertLig = Lambert(pin.normal, lightDirection.xyz,lightColor.xyz);
    float3 specularLig = Specular(cameraPos.xyz, pin.normal, lightDirection.xyz, pin.position.xyz,15);
    
    float3 lig = lambertLig + specularLig;
    
    float4 color = texture0.Sample(sampler0, pin.texcoord) * pin.color * materialColor;
    
    color.xyz *= lig;
  	// シャドウマップ適用
    if (dot(lightDirection.xyz, pin.normal.xyz) < 0)
    {
        color.xyz *= GetCascadeShadow(pin.Wposition.xyz);
    }
     

      
    return color;
}

