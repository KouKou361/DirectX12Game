Texture2D texture0 : register(t0);
Texture2D ShadowTexture0 : register(t1);
Texture2D ShadowTexture1 : register(t2);
Texture2D ShadowTexture2 : register(t3);

SamplerState sampler0 : register(s0);
SamplerState cascadeSmp : register(s2);
SamplerComparisonState shadowSmp : register(s1);



//3Dモデルを描画する定数バッファ
cbuffer CbScene : register(b0)
{
    float4x4            cameraView;
    float4x4            cameraProjection;
    float4              cameraPos;
    float4              cameraDirection;
    
    row_major float4x4  cameraViewProjection;
 
    float4              lightDirection;
    float4              lightColor;
    
    float4x4            shadowViewProjection;//通常シャドウ行列
    row_major float4x4  createCascadeShadowLightViewProjection[3]; //カスケードシャドウマップ行列
    float4x4            cascadeShadowLightViewProjection[3]; //カスケードシャドウマップ行列
  
};

//アニメーション
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[500];
};

//マテリアルデータ
cbuffer CbMaterial : register(b2)
{
    float4 materialColor;
};



//アニメションする3Dモデル頂点シェーダー受け取り
struct VS_IN
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float4 boneWeights : WEIGHTS;
    uint4 boneIndices : BONES;
};

//アニメションする3Dモデルピクセルシェーダー受け取り
struct PS_IN
{
    float4 position : SV_POSITION;
    float3 Wposition : TEXCOORD2;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

//アニメションする3Dモデルピクセルシェーダー受け取り
struct PS_ShadowIN
{
    float4 position : SV_POSITION;
    float4 Depth : TEXCOORD1; //深度情報
};



//ワールド座標をライト空間座標に変換
float3 GetShadowTex(float4x4 vp, float3 wPos)
{
 //正規化デバイス
    float4 wvpPos;
    wvpPos.xyz = wPos;
    wvpPos.w = 1.0f;
    
    wvpPos = mul(vp, wvpPos);
    wvpPos /= wvpPos.w;
    //テクスチャ
    wvpPos.y = -wvpPos.y;
    wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
    return wvpPos.xyz;
}

//シャドーマップからシャドー空間座標に変換とZ値比較
float3 GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor,
	float Bias)
{ // シャドウマップから深度を取り出す
	float d = st.Sample(ss, Tex.xy).r;
	// シャドウマップの深度値と現実の深度の比較
	Scolor = (Tex.z - d > Bias) ? Scolor : float3(1, 1, 1);
	return Scolor;
}

//ライト空間
float3 GetCascadeShadowTex(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor, float Bias)
{
    //ここで謎の警告！
    //反復を伴うループで使用される事で警告されているらしい。
    //for文を使わないようにしたら治りそうだが、
    //ソースコードの綺麗さを保つ為に今回はこの警告を逃しておく。
    
    //シャドウマップ
    float d = st.Sample(ss, Tex.xy).r;
    //シャドウマップマップの深度
    Scolor = (Tex.z - d > Bias) ? Scolor : float3(1, 1, 1);
    
    float2 da = st.Sample(ss, Tex.xy).rg;
    float R = da.x * da.x;
	//E(x2)
    float G = da.y;


	//分散の計算
    float variance = max(Bias, G - R);
	//確立上の最大値の算出
    float a = variance + (Tex.z - da.x) * (Tex.z - da.x);
    float p_max = saturate(variance / a);
	//影の色
    Scolor = lerp(Scolor, 1.0f, 0.1f);
   
    return Scolor;
    //return max(Scolor, p_max);

}



static const float CascadeBias[3] = { 0.0025f, 0.0022f, 0.0005f };
float3 GetCascadeShadow(float3 P)
{
     //シャドウマップ
    //Texture2D shadowTexure[3] = { ShadowTexture0, ShadowTexture1, ShadowTexture2 };

    float3 color = { 1, 1, 1 };

    for (int j = 0; j < 3; ++j)
    {
        
        float3 vShadow = GetShadowTex(cascadeShadowLightViewProjection[j], P);
        float3 ShadowTexcoord = vShadow;

        if (ShadowTexcoord.z >= 0 && ShadowTexcoord.z <= 1 &&
            ShadowTexcoord.x >= 0 && ShadowTexcoord.x <= 1 &&
            ShadowTexcoord.y >= 0 && ShadowTexcoord.y <= 1)
        {

            //switch文でするよりこちらの方がソースコードが綺麗になるが、
            //なぜか警告が出てしまう。（Texture2DArrayを使えば行けそうだが）
            //今回はswitch文にする。
            // 
            //color = GetShadow(shadowTexure[j], shadowmapSamplerState, vShadow, ShadowColor.xyz, Bias[j]);

            switch (j)
            {
                case 0:
                    color = GetCascadeShadowTex(ShadowTexture0, cascadeSmp, vShadow, float3(0.3f, 0.3f, 0.3f), CascadeBias[0]);
                    break;                                               
                case 1:                                                  
                    color = GetCascadeShadowTex(ShadowTexture1, cascadeSmp, vShadow, float3(0.3f, 0.3f, 0.3f), CascadeBias[1]);
                    break;                                              

                case 2:
                    color = GetCascadeShadowTex(ShadowTexture2, cascadeSmp, vShadow, float3(0.3f, 0.3f, 0.3f), CascadeBias[2]);
                    break;
            }
            break;
    
        }
    }
    return color;

}
//==============================
//ハーフランバート関数
//==============================
float3 Lambert(float3 N, float3 L, float3 K)
{
    float3 d = dot(N, L);
    d *= -1;
    d = max(0.5f, d);
    d *= K;
    
    return d;

}

//==============================
//鏡面反射関数
//==============================

float3 Specular(float3 eyePos, float3 normal, float3 lightDirection, float3 wPos,  float Power)
{
    float3 R = reflect(normal, lightDirection);
    R = normalize(R);
    
    float3 toEye = eyePos - wPos;
    toEye = normalize(toEye);
    
    float3 d = dot(R, lightDirection);

    d = max(0, d);
    
    d = pow(d, Power);
  

    return d;

}


static const float Bias = 0.005f;






