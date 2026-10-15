#include "PointSprite.hlsli"
[maxvertexcount(4)]
void main(
	point GSInput In[1],
	inout TriangleStream<PSInput> ParticleStream
)
{
	//座標変換（ワールド座標ー＞ビュー座標）
    float4 pos = mul(cameraView, float4(In[0].Position, 1.0f));
	//点を面にする
    float w = In[0].Size.x;
    float h = In[0].Size.y;
    
    float sx = sin(In[0].Rotation.x);
    float sy = cos(In[0].Rotation.x);
    
    float4 pos_left_top = pos +     float4(-w,  h, 0.0f, 0.0f); //左上
    float4 pos_left_bottom = pos +  float4(-w, -h, 0.0f, 0.0f); //左下
    float4 pos_right_top = pos +    float4( w,  h, 0.0f, 0.0f); //右上
    float4 pos_right_bottom = pos + float4( w, -h, 0.0f, 0.0f); //右下
    
	//UV。色を計算して出力
    PSInput Out = (PSInput) 0;
    uint type = int(In[0].Tex.x);
    
    float2 TextureNum = float2(1,1);
    float2 TextureSize = float2(1, 1);
    
	//UV座標
    float u = float(type % (uint) TextureNum.x) * TextureSize.x;
    float v = float(type / (uint) TextureNum.y) * TextureSize.y;
    
    Out.Color = In[0].Color;
	
	//左上
    Out.Position = mul(cameraProjection, pos_left_top);
    Out.Tex = float2(u, v);
    ParticleStream.Append(Out);
	//右上
    Out.Position = mul(cameraProjection, pos_right_top);
    Out.Tex = float2(u + TextureSize.x, v);
    ParticleStream.Append(Out);
    
	//左下
    Out.Position = mul(cameraProjection, pos_left_bottom);
    Out.Tex = float2(u, v + TextureSize.y);
    ParticleStream.Append(Out);
    
	//右下
    Out.Position = mul(cameraProjection, pos_right_bottom);
    Out.Tex = float2(u + TextureSize.x, v + TextureSize.y);
    ParticleStream.Append(Out);

    
    ParticleStream.RestartStrip();


}