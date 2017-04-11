#define COMPUTER_SHADER
#define GLOBAL_THREAD_DATA b0
#define GLOBAL_LIGHT_DATA b1
#include "CommonStructs.h"

static const uint GROUP_SIZE = TILE_SIZE * TILE_SIZE;
static const uint MAX_LIGHTS_PER_TILE = 512;

StructuredBuffer<Light> LightBuffer : register(t0);
StructuredBuffer<Frustum> FrustumBuffer : register(t1);
Texture2D DepthBuffer : register(t2);

RWStructuredBuffer<uint> LightIndexList : register(u0);
RWTexture2D<uint2> LightGrid : register(u1);
globallycoherent RWStructuredBuffer<uint> LightIndexListStart : register(u2);

groupshared uint TileLightCount;
groupshared uint TileLightList[MAX_LIGHTS_PER_TILE];
groupshared Frustum GroupFrustum;
groupshared uint LightIndexListOffset;
groupshared uint MinDepth; //Interlocked operations only work on integer types
groupshared uint MaxDepth; //Interlocked operations only work on integer types

bool CheckPlane(Plane p, Light l)
{
	float3 pos = l.Position - p.Normal * l.Range;
	float3 dir = normalize(p.Point - pos);

	return dot(p.Normal, dir) > 0.0f;
}

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void main(CSInput i)
{
	float4 depthColor = DepthBuffer.Load(int3(i.DispatchThreadID.xy, 0));
	uint depth = asuint(depthColor.x);

	if (i.GroupIndex == 0) // Only need one thread to initialise variables
	{
		TileLightCount = 0;
		GroupFrustum = FrustumBuffer[i.GroupID.x + (i.GroupID.y * 80)];
		MinDepth = 0xffffffff;
		MaxDepth = 0.0f;
	}

	GroupMemoryBarrierWithGroupSync();

	//Interlocked operations only work on integer types
	InterlockedMin(MinDepth, depth);
	InterlockedMax(MaxDepth, depth);

	GroupMemoryBarrierWithGroupSync();

	float minDepth = asfloat(MinDepth);
	float maxDepth = asfloat(MaxDepth);

	if (i.GroupIndex == 0)
	{
		float3 distance = GroupFrustum.Far.Point - GroupFrustum.Near.Point;
		GroupFrustum.Far.Point = GroupFrustum.Near.Point + distance * (maxDepth);
		GroupFrustum.Near.Point = GroupFrustum.Near.Point + distance * (minDepth);
	}

	GroupMemoryBarrierWithGroupSync();

	for (uint lightIndex = i.GroupIndex; lightIndex < NumOfLights; lightIndex += GROUP_SIZE)
	{
		//Make local copy of the light
		Light light = LightBuffer[lightIndex];

		if(CheckPlane(GroupFrustum.Left, light)
			&& CheckPlane(GroupFrustum.Right, light)
			&& CheckPlane(GroupFrustum.Top, light)
			&& CheckPlane(GroupFrustum.Bottom, light)
			&& CheckPlane(GroupFrustum.Far, light)
			&& CheckPlane(GroupFrustum.Near, light))
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