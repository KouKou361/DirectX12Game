#include "BasicShader.hlsli"

//アニメションする3Dモデルピクセルシェーダー受け取り
struct ShadowMapPS_IN
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float4 vShadow : TEXCOORD4;
};