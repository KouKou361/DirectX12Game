#include "PointSprite.hlsli"

GSInput main(VSInput input)
{

   
	//ジオメトリシェーダー側で座標変換するので頂点シェーダーはそのまま送る
    GSInput output = (GSInput) 0;
    
    const float3 addPos = input.SpeedVec * input.time;
    output.Position = input.Position + addPos;
    
    const float3 addScale = input.RotationVec * input.time;
    output.Size = input.Size + addScale;
    
    output.Rotation = input.Rotation * input.time;

    //= input.SpeedVec;
    //= input.AcceleVec;
    //= input.RotationVec;
    //;
    //= input.Dummy1;
    //= input.Dummy2;
    //
    output.Tex = input.Tex;
    
    const float4 addColor = input.AddColor * input.time;
    output.Color = input.Color + addColor;
    
  
    
    //
    return output;
}