
///////////////////////////
// Globals

cbuffer WorldTransforms
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjMatrix;
};

///////////////////////////
// Types

struct InputVS
{
	float3 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float2 UV		: TEXCOORD;
};

struct OutputVS
{
	float4 ProjPos	: SV_POSITION;
	float4 Pos		: POSITION;
	float4 Normal	: NORMAL;
	float2 UV		: TEXCOORD;
};

void main(in InputVS i, out OutputVS o)
{
	float4 modelPos = float4(i.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	float4 worldPos = mul(modelPos, WorldMatrix);
	o.Pos = worldPos;

	float4 viewPos = mul(worldPos, ViewMatrix);
	o.ProjPos = mul(viewPos,  ProjMatrix);

	float4 normal = float4(i.Pos, 1.0f);
	o.Normal = mul(normal, WorldMatrix);
	o.UV = i.UV;
}