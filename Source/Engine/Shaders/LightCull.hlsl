#define COMPUTER_SHADER
#define GLOBAL_THREAD_DATA b0
#define GLOBAL_LIGHT_DATA b1
#include "CommonStructs.h"

static const uint GROUP_SIZE = TILE_SIZE * TILE_SIZE;
static const uint MAX_LIGHTS_PER_TILE = 512;

StructuredBuffer<Light> LightBuffer : register(t0);
StructuredBuffer<Frustum> FrustumBuffer : register(t1);

RWStructuredBuffer<uint> LightIndexList : register(u0);
RWTexture2D<uint2> LightGrid : register(u1);
globallycoherent RWStructuredBuffer<uint> LightIndexListStart : register(u2);

groupshared uint TileLightCount;
groupshared uint TileLightList[MAX_LIGHTS_PER_TILE];
groupshared Frustum GroupFrustum;
groupshared uint LightIndexListOffset;

bool CheckPlane(Plane p, Light l)
{
	float3 pos = l.Position - p.Normal * l.Range;
	float3 dir = normalize(p.Point - pos);

	return dot(p.Normal, dir) > 0.0f;
}

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void main(CSInput i)
{
	if (i.GroupIndex == 0) // Only need one thread to initialise variables
	{
		TileLightCount = 0;
		GroupFrustum = FrustumBuffer[i.GroupID.x + (i.GroupID.y * 80)];
	}

	GroupMemoryBarrierWithGroupSync();

	for (uint lightIndex = i.GroupIndex; lightIndex < NumOfLights; lightIndex += GROUP_SIZE)
	{
		//Make local copy of the light
		Light light = LightBuffer[lightIndex];

		if(CheckPlane(GroupFrustum.Left, light)
			&& CheckPlane(GroupFrustum.Right, light)
			&& CheckPlane(GroupFrustum.Top, light)
			&& CheckPlane(GroupFrustum.Bottom, light))
		//if((i.GroupID.x + i.GroupID.y) % 2 == 0)
		{
			uint tileLightListIndex;
			InterlockedAdd(TileLightCount, 1, tileLightListIndex);
			if(tileLightListIndex < MAX_LIGHTS_PER_TILE) TileLightList[tileLightListIndex] = lightIndex;
		}
	}

	GroupMemoryBarrierWithGroupSync();

	if (i.GroupIndex == 0)
	{
		LightIndexListOffset = 0;
		InterlockedAdd(LightIndexListStart[0], TileLightCount, LightIndexListOffset);
		LightGrid[i.GroupID.xy] = uint2(LightIndexListOffset, TileLightCount);
	}

	GroupMemoryBarrierWithGroupSync();

	for (uint index = i.GroupIndex; index < TileLightCount; index += GROUP_SIZE)
	{
		LightIndexList[LightIndexListOffset + index] = TileLightList[index];
	}

	GroupMemoryBarrierWithGroupSync();
}