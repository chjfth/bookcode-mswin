float4x4 WorldViewProj;
sampler DiffuseSampler;

void TX_HLL_EX1(
	in  float2 vTex : POSITION,
	out float4 oCol : COLOR0)
{
	oCol = float4(vTex.x, 0, 0, 0);

	// horizontal lines
	if( (0.25 < vTex.y)  && (vTex.y < 0.30) )
		oCol.x = 0;
	else if( (0.50 < vTex.y) && (vTex.y < 0.55) )
		oCol.x = 0;
	else if( (0.75 < vTex.y) && (vTex.y < 0.80) )
		oCol.x = 0;


	// vertical lines
	if( (0.40 < vTex.x) && (vTex.x < 0.42) )
		oCol.x = 0;
	else if( (0.50 < vTex.x) && (vTex.x < 0.52) )
		oCol.x = 0;
	else if( (0.60 < vTex.x) && (vTex.x < 0.62) )
		oCol.x = 0;
	else if( (0.70 < vTex.x) && (vTex.x < 0.72) )
		oCol.x = 0;
	else if( (0.80 < vTex.x) && (vTex.x < 0.82) )
		oCol.x = 0;
	else if( (0.90 < vTex.x) && (vTex.x < 0.92) )
		oCol.x = 0;
     
}

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
