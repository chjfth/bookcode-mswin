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
	// texture
	oCol = tex2D(DiffuseSampler, vTex);

	// 2a - complement
	oCol = 1.0 - oCol;

	// 2b - darken
	//oCol = 0.5 * tex2D(DiffuseSampler, vTex);

	// 2c - no red
	//oCol = tex2D(DiffuseSampler, vTex); 
	//oCol.r = 0;

	// 2d - red only; green, blue, alpha mask
	//oCol = tex2D(DiffuseSampler, vTex); 
	//oCol.g = oCol.b = oCol.a = 0;
}
