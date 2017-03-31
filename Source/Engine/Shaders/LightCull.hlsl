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

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void main(CSInput i)
{
	if (i.GroupIndex == 0) // Only need one thread to initialise variables
	{
		TileLightCount = 0;
		//GroupFrustum = FrustumBuffer[i.GroupID.x + (i.GroupID.y * NumOfThreadGroups.x)];
	}

	GroupMemoryBarrierWithGroupSync();

	for (uint lightIndex = i.GroupIndex; lightIndex < NumOfLights; lightIndex += GROUP_SIZE)
	{
		//Make local copy of the light
		Light light = LightBuffer[lightIndex];

		//if() cull code
		//{
			uint tileLightListIndex;
			InterlockedAdd(TileLightCount, 1, tileLightListIndex);
			if(tileLightListIndex < MAX_LIGHTS_PER_TILE) TileLightList[tileLightListIndex] = lightIndex;
		//}
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