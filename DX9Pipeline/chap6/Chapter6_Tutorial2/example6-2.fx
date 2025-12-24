float4x4 WorldViewProj;
sampler DiffuseSampler;

void VS_HLL_EX1(
    in  float4 vPos  : POSITION, 
    in  float2 vTex  : TEXCOORD0, 
    out float4 oPos  : POSITION,
    out float2 oTex  : TEXCOORD0)
{
    oPos = mul(vPos, WorldViewProj);
    oTex = vTex;
}

void PS_HLL_EX1(
    in  float2 vTex  : TEXCOORD0,
    out float4 oCol  : COLOR0)
{
    oCol = tex2D(DiffuseSampler, vTex);
}
