
static const uint TILE_SIZE = 16;

///////////////////////////
// Globals/Constant Buffers

//Lighting meta data
cbuffer GlobalLightData : register(b0)
{
	float4 AmbientColour	: packoffset(c0);
	float4 CameraPos		: packoffset(c1);
	float SpecularPower		: packoffset(c2);
	uint NumOfLights		: packoffset(c2.y);
	float2 Padding			: packoffset(c2.z);
};

cbuffer GlobalThreadData : register(b1)
{
	float4 NumOfThreads			: packoffset(c0);
	float4 NumOfThreadGroups	: packoffset(c1);
}

///////////////////////////
// Types

struct Light
{
	float3 Position;
	float Brightness;//Be careful of 16 byte boundaries
	float3 Colour;
	float Range;
};

struct Plane
{
	float3 Normal;
	float Padding;
};

struct Frustum
{
	Plane Planes[4];
};

//Struct taken from 3dgep.com
struct ComputeShaderInput
{
	uint3 groupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
	uint3 groupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
	uint3 dispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
	uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

StructuredBuffer<Light> LightBuffer : register(t0);





[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

}