#define GLOBAL_LIGHT_DATA b0
#define MATERIAL_DATA b1

#include "CommonStructs.h"

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

Texture2D DiffuseTexture : register(t0);
Texture2D SpecularTexture : register(t1);
StructuredBuffer<Light> LightBuffer : register(t2);

SamplerState TextureSampler;



void main(in InputPS i, out OutputPS o)
{
	float3 WorldNormal = normalize(i.WorldNormal.xyz);

	////////////////////////
	// Lighting preparation

	// Get normalised vector to camera for specular equation
	float3 CameraDir = normalize(CameraPos.xyz - i.WorldPos.xyz);

	// Accumulate diffuse and specular colour effect from each light
	float3 TotalDiffuseColour = AmbientColour.rgb;
	float3 TotalSpecularColour = 0;

	for (uint light = 0; light < NumOfLights; ++light)
	{
		// Calculate diffuse lighting from the light. Equation: Diffuse = light colour * max(0, N.L)
		float3 LightDir = LightBuffer[light].Position - i.WorldPos.xyz;
		float LightDist = length(LightDir);
		float LightBrightness = LightBuffer[light].Brightness;
		LightDir /= LightDist;
		float LightStrength = saturate(LightBrightness / LightDist);
		float3 DiffuseColour = LightStrength * LightBuffer[light].Colour * saturate(dot(WorldNormal, LightDir));
		TotalDiffuseColour += DiffuseColour * smoothstep(0.0f, LightBuffer[light].Range, LightBuffer[light].Range - LightDist);

		// Calculate specular lighting from the 1st light. Standard equation: Specular = light colour * max(0, (N.H)^p)
		float3 Halfway = normalize(CameraDir + LightDir);
		TotalSpecularColour += DiffuseColour * saturate(pow(dot(WorldNormal, Halfway), SpecularPower)) * Shinyness
			* smoothstep(0.0f, LightBuffer[light].Range * 3.0f, LightBuffer[light].Range * 3.0f - LightDist);
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