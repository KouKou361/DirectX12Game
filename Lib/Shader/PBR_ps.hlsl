#include "PBR.hlsli"

Texture2D NormalTexture : register(t4);
Texture2D MetallicTexture : register(t5);
Texture2D SmoothnessTexture : register(t6);

static const float PI = 3.1415926f; // π

float3 GetNormal(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
    float3 binSpaceNormal = NormalTexture.SampleLevel(sampler0, uv, 0.0f).xyz;
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

    float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;

    return newNormal;
}

// ベックマン分布を計算する
float Beckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0f / (4.0f * m2 * t4);
    D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
    return D;
}

// フレネルを計算。Schlick近似を使用
float SpcFresnel(float f0, float u)
{
    // from Schlick
    return f0 + (1 - f0) * pow(1 - u, 5);
}

/// <summary>
/// Cook-Torranceモデルの鏡面反射を計算
/// </summary>
/// <param name="L">光源に向かうベクトル</param>
/// <param name="V">視点に向かうベクトル</param>
/// <param name="N">法線ベクトル</param>
/// <param name="metallic">金属度</param>
float CookTorranceSpecular(float3 L, float3 V, float3 N, float metallic)
{
    float microfacet = 0.76f;

    // 金属度を垂直入射の時のフレネル反射率として扱う
    // 金属度が高いほどフレネル反射は大きくなる
    float f0 = metallic;

    // ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
    float3 H = normalize(L + V);

    // 各種ベクトルがどれくらい似ているかを内積を利用して求める
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));

    // D項をベックマン分布を用いて計算する
    float D = Beckmann(microfacet, NdotH);

    // F項をSchlick近似を用いて計算する
    float F = SpcFresnel(f0, VdotH);

    // G項を求める
    float G = min(1.0f, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));

    // m項を求める
    float m = PI * NdotV * NdotH;

    // ここまで求めた、値を利用して、Cook-Torranceモデルの鏡面反射を求める
    return max(F * D * G / m, 0.0);
}
/// <summary>
/// フレネル反射を考慮した拡散反射を計算
/// </summary>
/// <remark>
/// この関数はフレネル反射を考慮した拡散反射率を計算します
/// フレネル反射は、光が物体の表面で反射する現象のとこで、鏡面反射の強さになります
/// 一方拡散反射は、光が物体の内部に入って、内部錯乱を起こして、拡散して反射してきた光のことです
/// つまりフレネル反射が弱いときには、拡散反射が大きくなり、フレネル反射が強いときは、拡散反射が小さくなります
///
/// </remark>
/// <param name="N">法線</param>
/// <param name="L">光源に向かうベクトル。光の方向と逆向きのベクトル。</param>
/// <param name="V">視線に向かうベクトル。</param>
float CalcDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
    // step-4 フレネル反射を考慮した拡散反射光を求める

    // 法線と光源に向かうベクトルがどれだけ似ているかを内積で求める
    float dotNL = saturate(dot(N, L));

    // 法線と視線に向かうベクトルがどれだけ似ているかを内積で求める
    float dotNV = saturate(dot(N, V));

    // 法線と光源への方向に依存する拡散反射率と、法線と視点ベクトルに依存する拡散反射率を
    // 乗算して最終的な拡散反射率を求めている。PIで除算しているのは正規化を行うため
    return (dotNL * dotNV);
}




float4 main(ShadowMapPS_IN pin) : SV_TARGET
{
   // 法線を計算
    float3 normal = GetNormal(pin.VZ, pin.VX, pin.VY, pin.texcoord);
  //  normal = normalize(normal);

    float3 LightDirection = normalize(lightDirection);
    

    // step-2 各種マップをサンプリングする
    // アルベドカラー（拡散反射光）
    float4 albedoColor = texture0.Sample(sampler0, pin.texcoord);

    // スペキュラカラーはアルベドカラーと同じにする
    float3 specColor = albedoColor;
    
    // 金属度
    float metallic = MetallicTexture.Sample(sampler0, pin.texcoord).r;

    // 滑らかさ
    float smooth = SmoothnessTexture.Sample(sampler0, pin.texcoord).r;

    // 視線に向かって伸びるベクトルを計算する
    float3 toEye = normalize(cameraPos.xyz - pin.Wposition.xyz);

    float3 lig = 0;

   // step-3 シンプルなディズニーベースの拡散反射を実装する
   // フレネル反射を考慮した拡散反射を計算
   float diffuseFromFresnel = CalcDiffuseFromFresnel(
       normal, LightDirection.xyz, toEye);

   // 正規化Lambert拡散反射を求める
    float NdotL = saturate(dot(normal, LightDirection.xyz));
   float3 lambertDiffuse = lightColor * NdotL / PI;

   // 最終的な拡散反射光を計算する
   float3 diffuse = albedoColor.xyz * diffuseFromFresnel * lambertDiffuse;

   // step-5 Cook-Torranceモデルを利用した鏡面反射率を計算する
   // Cook-Torranceモデルの鏡面反射率を計算する
   float3 spec = CookTorranceSpecular(
       LightDirection.xyz, toEye, normal, smooth)
       * lightColor;

   // 金属度が高ければ、鏡面反射はスペキュラカラー、低ければ白
   // スペキュラカラーの強さを鏡面反射率として扱う
    spec *= lerp(float3(0.5f, 0.5f, 0.5f), specColor, metallic);

   // step-6 滑らかさを使って、拡散反射光と鏡面反射光を合成する
   // 滑らかさが高ければ、拡散反射は弱くなる
   lig += diffuse * (1.0f - smooth) + spec;

    
    
    float3 ambientLight = float3(0.5f, 0.5f, 0.5f);
    // 環境光による底上げ
    lig += ambientLight * albedoColor.xyz;

    float4 finalColor = 1.0f;
    finalColor.xyz = lig;
    
    {
        finalColor.xyz *= GetCascadeShadow(pin.Wposition.xyz);
    }
    
    float3 lambertLig = Lambert(pin.normal, lightDirection.xyz, lightColor.xyz);
    
    if (lambertLig.x <= 0.5f)
    {
        //finalColor.x *= lambertLig.x;
        //finalColor.y *= lambertLig.y;
        //finalColor.z *= lambertLig.z;
        //
        
        finalColor.xyz = max(lambertLig, 0.5f) * lig;
    }
     
    return finalColor;
    

}

