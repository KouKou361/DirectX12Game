                                           
#include "PBR.hlsli"


ShadowMapPS_IN main(VS_IN input, uint instNo : SV_InstanceID)
{
//#if 1
//    float3 p = { 0, 0, 0 };
//    float3 n = { 0, 0, 0 };
//    
//    
//    float4x4 world = 0.0f;
//    
//    for (int i = 0; i < 4; i++)
//    {
//        if (instNo == 0)
//        {
//            p += (input.boneWeights[i] * mul(input.position, boneTransforms[input.boneIndices[i]])).xyz;
//            n += (mul(float4(input.normal.xyz, 0), boneTransforms[input.boneIndices[i]])).xyz;
//            //world += boneTransforms[input.boneIndices[i]];
//        }
//        else
//        {
//            p += (input.boneWeights[i] * mul(input.position, boneTransforms[instNo])).xyz;
//            n += (mul(float4(input.normal.xyz, 0), boneTransforms[input.boneIndices[i]])).xyz;
//            //world += boneTransforms[input.boneIndices[i]];
//        }
//            
//    }
//#else
//    float3 p = input.position.xyz;
//    float3 n = input.normal;
//#endif
//        ShadowMapPS_IN vout;
//    vout.position = mul(float4(p, 1.0f), cameraViewProjection);
//    vout.color = input.color;
//    vout.texcoord = input.texcoord;
//    vout.Wposition = p;
//    vout.normal = normalize(n);
//    
//    //Ú‹óŠÔ‚RŽ²‚Ìì¬
//    float3 VZ = mul(vout.normal.xyz, world);
//    float3 VY = { 0, 1, 0.001 }; //‰¼
//    float3 VX;
//    VY = normalize(VY);
//    VX = normalize(cross(VY, VZ)); //ŠOÏ
//    VY = normalize(cross(VZ, VX));
//
//    vout.VX = VX;
//    vout.VY = VY;
//    vout.VZ = VZ;
//

//    
//    return vout;
    
#if 1
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    row_major float4x4 world = 
    {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    
 
    
    for (int i = 0; i < 4; i++)
    {
        if (instNo == 0)
        {
            p += (input.boneWeights[i] * mul(input.position, boneTransforms[input.boneIndices[i]])).xyz;
            n += (input.boneWeights[i] * mul(float4(input.normal.xyz, 0), boneTransforms[input.boneIndices[i]])).xyz;
        }
        else
        {
            p += (input.boneWeights[i] * mul(input.position, boneTransforms[instNo])).xyz;
            n += (input.boneWeights[i] * mul(float4(input.normal.xyz, 0), boneTransforms[instNo])).xyz;
        }
            
    }
#else
    float3 p = input.position.xyz;
    float3 n = input.normal;
#endif
    ShadowMapPS_IN vout;
    vout.position = mul(float4(p, 1.0f), cameraViewProjection);
    vout.color = input.color;
    vout.texcoord = input.texcoord;
    vout.Wposition = p;
    vout.normal = normalize(n);
    
        //Ú‹óŠÔ‚RŽ²‚Ìì¬
    float3 VZ = normalize(vout.normal);
    float3 VY = { 0, 1, 0.001 }; //‰¼
    float3 VX;
    VY = normalize(VY);
    VX = normalize(cross(VY, VZ)); //ŠOÏ
    VY = normalize(cross(VZ, VX));
    
    //vout.position.x += VX*0.5f ;
    //vout.position.y += VY * 0.5f;
    //vout.position.z += VZ * 0.5f;
   // vout.position.xyz += VZ*0.5f ;

    vout.VX = VX;
    vout.VY = VY;
    vout.VZ = VZ;

    return vout;
}
