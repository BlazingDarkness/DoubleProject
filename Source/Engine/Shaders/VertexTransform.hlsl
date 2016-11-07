
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

struct BasicVertInput
{
	float3 Pos : POSITION;
};

struct BasicVertOutput
{
	float4 ProjPos		: SV_POSITION;
};


///////////////////////////
// Shader

BasicVertOutput main( BasicVertInput vIn)
{
	BasicVertOutput vOut;

	float4 modelPos	= float4( vIn.Pos, 1.0f );
	float4 worldPos	= mul( modelPos, WorldMatrix );
	float4 viewPos	= mul( worldPos, ViewMatrix );
	vOut.ProjPos	= mul( viewPos, ProjMatrix );

	return vOut;
}

/*
float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}*/