
///////////////////////////
// Globals

cbuffer GlobalMatrix : register(b0)
{
	row_major float4x4 ViewMatrix;
	row_major float4x4 ProjMatrix;
};

cbuffer ObjectMatrix : register(b1)
{
	float4x4 WorldMatrix;
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
};

void main(in InputVS i, out OutputVS o)
{
	float4 modelPos = float4(i.Pos, 1.0f);
	float4 worldPos = mul(modelPos, WorldMatrix);
	float4 viewPos = mul(worldPos, ViewMatrix);
	o.ProjPos = mul(viewPos, ProjMatrix);
}