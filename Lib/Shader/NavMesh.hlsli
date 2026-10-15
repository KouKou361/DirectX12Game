
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

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


//アニメーション
cbuffer CbMesh : register(b1)
{
    row_major float4x4 world;
};

