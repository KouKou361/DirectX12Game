Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);


//3Dモデルを描画する定数バッファ
cbuffer CbScene : register(b0)
{
    float4x4 cameraView;
    float4x4 cameraProjection;
    float4 cameraPos;
    float4 cameraDirection;
    
    row_major float4x4 cameraViewProjection;
 
    float4 lightDirection;
    float4 lightColor;
    
    float4x4 shadowViewProjection; //通常シャドウ行列
    row_major float4x4 createCascadeShadowLightViewProjection[3]; //カスケードシャドウマップ行列
    float4x4 cascadeShadowLightViewProjection[3]; //カスケードシャドウマップ行列
  
};

struct VSInput
{
    float3 Position     : POSITION;//原点位置
    float3 Size         : TEXCOORD_; //拡大
    float3 Rotation     : TEXCOORD1_; //回転
    
    float3 SpeedVec     : TEXCOORD2_;//移動量
    float3 AcceleVec    : TEXCOORD3_;//加速度
    float3 RotationVec  : TEXCOORD4_;//回転量
    float3 Scale        : TEXCOORD5_;//拡大量
    float4 AddColor     : TEXCOORD6_; //予備
    float3 Dummy2       : TEXCOORD7_;//予備
    
    float4 Tex          : TANGENT; //切り取り位置
    float4 Color        : COLOR; //色
    float  time         : TEXCOORD8_;//予備
    
};
struct GSInput
{
    float3 Position : POSITION; //位置
    float2 Size : TEXCOORD; //大きさ
    float4 Tex : TANGENT; //タイプ,空,空,空
    float4 Color : COLOR; //色
    float3 Rotation : TEXCOORD1_; //回転
};
struct PSInput
{
    float4 Position : SV_POSITION; //頂点座標
    float2 Tex : TETCOORD; //テクスチャ座標
    float4 Color : COLOR0;
};