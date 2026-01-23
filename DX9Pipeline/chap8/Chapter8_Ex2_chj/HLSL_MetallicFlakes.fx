//
// Metallic Flakes Shader
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Note: This effect file works with EffectEdit.
//
// NOTE:
// The metallic surface consists of 2 layers.
// 1. a polished layer of wax on top (contributes a smooth specular reflection and 
//    an environment mapped reflection with a Fresnel term), and 
// 2. a bronze metallic layer with a sprinkling of gold metallic flakes underneath

// Sparkle parameters
#define SPRINKLE    0.3
#define SCATTER     0.3

#define VOLUME_NOISE_SCALE  10

// Transformations
float4x3 WorldView  : WORLDVIEW;
float4x4 Projection : PROJECTION;

// Light direction (view space)
float3 L <string UIDirectional="Light Direction";> = 
	normalize(float3(1, 0, 1));  // chj: light-to direction (view space)

// Light intensity
float4 I_a = { 0.3f, 0.3f, 0.3f, 1.0f };    // ambient
float4 I_d = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse
float4 I_s = { 0.7f, 0.7f, 0.7f, 1.0f };    // specular

// Material reflectivity
float4 k_a : MATERIAL_AMBIENT = { 0.2f, 0.2f, 0.2f, 1.0f };  // ambient (metal, grey)
float4 k_d : MATERIAL_DIFFUSE = { 0.7f, 0.7f, 0.1f, 1.0f };  // diffuse (metal, bronze)
float4 k_s = { 0.4f, 0.3f, 0.1f, 1.0f };    // specular (metal)
float4 k_r = { 0.7f, 0.7f, 0.7f, 1.0f };    // specular (wax)


struct VS_OUTPUT
{
	float4 Position   : POSITION;
	float3 Diffuse    : COLOR0;
	float3 Specular   : COLOR1;   
	float3 NoiseCoord : TEXCOORD0;
	float3 Reflection : TEXCOORD1;
	float3 Glossiness : TEXCOORD2;
	float3 HalfVector : TEXCOORD3;
};

// Vertex shader
VS_OUTPUT VS_Sparkle(    
	float3 Position : POSITION,
	float3 Normal   : NORMAL,
	float3 Tangent  : TANGENT)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	L = -L;

	float3 P = mul(float4(Position, 1), (float4x3)WorldView); // position (view space)
	float3 V = -normalize(P);                        // view direction (view space)
	
	float3 N = normalize(mul(Normal, (float3x3)WorldView));   // normal (view space)
	float3 T = normalize(mul(Tangent, (float3x3)WorldView));  // tangent (view space)
	float3 B = cross(N, T);                                   // binormal (view space)
	
//	float3 R = normalize(2 * dot(N, L) * N - L);  // reflection vector (view space)
	float3 G = normalize(2 * dot(N, V) * N - V);  // glance vector (view space)
	
	float3 H = normalize(L + V);                  // half vector (view space)
	
	// Position (projected)
	Out.Position = mul(float4(P, 1), Projection);

	// Diffuse + ambient (metal)
	Out.Diffuse = I_a * k_a + I_d * k_d * max(0, dot(N, L)); 

	// Specular (wax)
	Out.Specular  = saturate(dot(H, N)); // clamp `dot(H, N)` to [0, 1]
	Out.Specular *= Out.Specular;
	Out.Specular *= Out.Specular;
	Out.Specular *= Out.Specular;
	Out.Specular *= Out.Specular;
	Out.Specular *= Out.Specular;
	Out.Specular *= k_r;

	 // Glossiness (wax)
	float f;
	f = 0.5 - dot(N, V); 
	f = 1 - 4 * f * f;   // fresnel term
	Out.Glossiness = f * k_r;

	// Transform half vector into tangent-space
	Out.HalfVector = float3(dot(H, N), dot(H, B), dot(H, T));
	Out.HalfVector = (1 + Out.HalfVector) / 2;  // bias, clamp from [-1, 1] to [0, 1]

	// Environment cube map coordinates
	Out.Reflection = float3(-G.x, G.y, -G.z);

	// Volume noise coordinates
	Out.NoiseCoord = Position * VOLUME_NOISE_SCALE;

	return Out;
}


/////////// Procedural Texture /////////////////////

// Function used to fill the volume noise texture
float4 GenerateSparkle(float3 Pos : POSITION) : COLOR
{
	float4 Noise = (float4)0;

	// Scatter the normal (in tangent-space) based on SCATTER
	Noise.rgb = float3(1 - SCATTER * abs(noise(Pos * 500)), 
					   SCATTER * noise((Pos + 1) * 500), 
					   SCATTER * noise((Pos + 2) * 500));
	Noise.rgb = normalize(Noise.rgb);

	// Set the normal to zero with a probability based on SPRINKLE
	if (SPRINKLE < abs(noise(Pos * 600)))
		Noise.rgb = 0;

	// Bias the normal
	Noise.rgb = (Noise.rgb + 1)/2;

	return Noise;
}


/////////// PS ////////////

sampler SparkleNoise : register(s0);  // ass: m_pd3dDevice->SetTexture(0, m_pNoiseMap);

sampler Environment : register(s1);   // ass: m_pd3dDevice->SetTexture(1, m_pEnvironmentMap);


// Pixel shader
float4 PS_Sparkle(VS_OUTPUT In) : COLOR
{   
	float4 Color = (float4)0;
	float3 Diffuse = In.Diffuse;     // Chj: even 0, the bigship still has some color
	float3 Specular, Gloss, Sparkle;

	// Volume noise
	float4 Noise = tex3D(SparkleNoise, In.NoiseCoord);
	
	// Glossy specular of wax
	Specular  = In.Specular;
	Specular *= Specular;
	Specular *= Specular;
	
	// Glossy reflection of wax 
	Gloss = texCUBE(Environment, In.Reflection) * saturate(In.Glossiness);

	// Specular sparkle of flakes
	Sparkle  = saturate(dot(
		(saturate(In.HalfVector) - 0.5) * 2, 
		(Noise.rgb - 0.5) * 2
		));
	Sparkle *= Sparkle;
	Sparkle *= Sparkle;
	Sparkle *= Sparkle;
	Sparkle *= k_s;

	// Combine the contributions
	Color.rgb = Diffuse + Specular + Gloss + Sparkle;
	Color.w   = 1;

	return Color;
}  
