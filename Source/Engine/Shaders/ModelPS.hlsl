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
StructuredBuffer<uint> LightIndexList : register(t3);
Texture2D<uint2> LightGrid : register(t4);

SamplerState TextureSampler;



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

	uint2 Tile = uint2((uint)(i.ScreenPos.x + 15.0f), (uint)(i.ScreenPos.y + 15.0f)) / 16;
	uint TileStart = LightGrid[Tile].x;
	uint TileEnd = TileStart + LightGrid[Tile].y;

	for (uint index = TileStart; index < TileEnd; ++index)
	{
		uint light = LightIndexList[index];
		// Calculate diffuse lighting from the light. Equation: Diffuse = light colour * max(0, N.L)
		float3 LightDir = LightBuffer[light].Position - i.WorldPos.xyz;
		float LightDist = length(LightDir);
		float LightBrightness = LightBuffer[light].Brightness;
		LightDir /= LightDist;
		float LightStrength = saturate(LightBrightness / LightDist);
		float3 DiffuseColour = LightStrength * LightBuffer[light].Colour * saturate(dot(WorldNormal, LightDir));
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