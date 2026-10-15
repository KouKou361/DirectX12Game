#include "PointSprite.hlsli"
float4 main(PSInput input) : SV_TARGET
{

    
    
        float4 color = (float4) 0;
    color = DiffuseTexture.Sample(DecalSampler, input.Tex) * input.Color /** input.Color*/;
   // color = input.Color;

  
    
    return color;
}