#include "BasicShader.hlsli"

//アニメションする3Dモデルピクセルシェーダー受け取り
struct ShadowMapPS_IN
{
    float4 position : SV_POSITION;
    float3 Wposition : TEXCOORD2;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 normal : NORMAL;
    
    
    
    float3 VX : TEXCOORD3; //接空間用X軸
    float3 VY : TEXCOORD4; //接空間用Y軸
    float3 VZ : TEXCOORD5; //接空間用Z軸
    

};

