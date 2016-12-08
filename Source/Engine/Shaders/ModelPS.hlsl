
cbuffer GlobalLightData : register(b0)
{
	float4 AmbientColour	: packoffset(c0);
	float4 CameraPos		: packoffset(c1);
	float SpecularPower		: packoffset(c2);
	uint NumOfLights		: packoffset(c2.y);
	float2 Padding			: packoffset(c2.z);
};

cbuffer MaterialData : register(b1)
{
	float4 DiffuseColour	: packoffset(c0);
	float Alpha				: packoffset(c1);
	float Dirtyness			: packoffset(c1.y);
	float Shinyness			: packoffset(c1.z);
	uint HasAlpha			: packoffset(c1.w);
	uint HasDirt			: packoffset(c2);
	uint HasDiffuseTex		: packoffset(c2.y);
	uint HasSpecTex			: packoffset(c2.z);
	float MaterialPadding	: packoffset(c2.w);
}

Texture2D DiffuseTexture : register(t0);
Texture2D SpecularTexture : register(t1);
StructuredBuffer<float4> LightPosAndBrightnessBuffer : register(t2);
StructuredBuffer<float4> LightColourBuffer : register(t3);

SamplerState TextureSampler;

struct InputPS
{
	float4 ScreenPos	: SV_POSITION;
	float4 WorldPos		: POSITION;
	float4 WorldNormal	: NORMAL;
	float2 UV			: TEXCOORD;
};

struct OutputPS
{
	float4 Colour : SV_TARGET;
};


void main( in InputPS i, out OutputPS o)
{
	// Renormalise world normal for the pixel as it has been interpolated from the vertex world normals & may not be length 1
	float3 WorldNormal = normalize(i.WorldNormal.xyz);

	////////////////////////
	// Lighting preparation

	// Get normalised vector to camera for specular equation (common for all lights)
	float3 CameraDir = normalize(CameraPos.xyz - i.WorldPos.xyz);

	// Accumulate diffuse and specular colour effect from each light
	float3 TotalDiffuseColour = AmbientColour.rgb;
	float3 TotalSpecularColour = 0;

	for (uint light = 0; light < NumOfLights; ++light)
	{
		// Calculate diffuse lighting from the light. Equation: Diffuse = light colour * max(0, N.L)
		float3 LightDir = LightPosAndBrightnessBuffer[light].xyz - i.WorldPos.xyz;
		float LightDist = length(LightDir);
		float LightBrightness = LightPosAndBrightnessBuffer[light].w;
		LightDir /= LightDist;
		float LightStrength = saturate(LightBrightness / LightDist);
		float3 DiffuseColour = LightStrength * LightColourBuffer[light].xyz * saturate(dot(WorldNormal, LightDir));
		TotalDiffuseColour += DiffuseColour;
		

		// Calculate specular lighting from the 1st light. Standard equation: Specular = light colour * max(0, (N.H)^p)
		// Slight tweak here: multiply by diffuse colour rather than light colour
		float3 Halfway = normalize(CameraDir + LightDir);
		TotalSpecularColour += DiffuseColour * saturate(pow(dot(WorldNormal, Halfway), SpecularPower)) * Shinyness;
	}

	////////////////////////
	// Final blending

	if (HasDiffuseTex == 1)
	{
		// Combine lighting colours with texture - alpha channel of texture is a specular map
		float4 TextureColour = DiffuseTexture.Sample(TextureSampler, i.UV);
		o.Colour.rgb = TotalDiffuseColour * TextureColour.rgb + TotalSpecularColour * TextureColour.a;
	}
	else
	{
		o.Colour.rgb = TotalDiffuseColour * DiffuseColour.rgb + TotalSpecularColour * Shinyness;
	}

	// Set alpha blending to 1 (no alpha available in texture)
	o.Colour.a = 1.0f;
}