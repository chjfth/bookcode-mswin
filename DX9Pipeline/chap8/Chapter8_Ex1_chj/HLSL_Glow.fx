
float4x3 WorldView  : WORLDVIEW;  
float4x4 Projection : PROJECTION; 
 
static float3 LightDir < string UIDirectional = "Light Direction"; > = 
normalize(float3(1.0f, 0.0f, -1.0f)); 
 
struct VS_OUTPUT 
{ 
	float4 Position : POSITION; 
	float4 Diffuse  : COLOR; 
	float2 TexCoord : TEXCOORD0; 
}; 
 
VS_OUTPUT VS_HLSL_Texture( 
	float4 Position : POSITION,  
	float3 Normal   : NORMAL, 
	float2 TexCoord : TEXCOORD0 
) 
{ 
	VS_OUTPUT Out; 
 
	float3 L = LightDir;                             // light direction (view space) 
	float3 P = mul(Position, WorldView);                    // position (view space) 
	float3 N = normalize(mul(Normal, (float3x3)WorldView)); // normal (view space) 
 
	Out.Position = mul(float4(P, 1), Projection);   // projected position 
	Out.Diffuse  = max(0, dot(N, L)); // diffuse  
	Out.TexCoord = TexCoord;                        // texture coordinates 
 
	return Out; 
} 

/////////////// GLOW ///////////////

static float4 GlowColor     = float4(0.5f, 0.2f, 0.2f, 1.0f); 
static float4 GlowAmbient   = float4(0.2f, 0.2f, 0.0f, 0.0f); 
static float  GlowThickness = 0.015f; 

 
struct VSGLOW_OUTPUT 
{ 
	float4 Position : POSITION; 
	float4 Diffuse  : COLOR; 
};

// Draw the glow 
VSGLOW_OUTPUT VS_HLSL_Glow 
	( 
	float4 Position : POSITION,  
	float3 Normal   : NORMAL 
	) 
{ 
	VSGLOW_OUTPUT Out; 
 
	float3 N = normalize(mul(Normal, (float3x3)WorldView));     // normal (view space) 
	float3 P = mul(Position, WorldView) + GlowThickness * N;    // displaced position (view space) 
	float3 A = float3(0, 0, 1);                                 // glow axis 
 
	float Power; 
 
	Power  = dot(N, A); 
	Power *= Power; 
	Power -= 1; 
	Power *= Power;     // Power = (1 - (N.A)^2)^2 [ = ((N.A)^2 - 1)^2 ] 
	 
	Out.Position = mul(float4(P, 1), Projection);   // projected position 
	Out.Diffuse  = GlowColor * Power + GlowAmbient; // modulated glow color + glow ambient 
	
	return Out; 
}
