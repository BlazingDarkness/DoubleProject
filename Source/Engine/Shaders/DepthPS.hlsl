#define FRUSTUM_DATA b0
#include "CommonStructs.h"

///////////////////////////
// Types

struct InputPS
{
	float4 ProjPos	: SV_POSITION;
	float4 ViewPos	: VIEW_POSITION;
};

struct OutputPS
{
	float4 Colour : SV_TARGET;
};

///////////////////////////
// Shader

//Calculates the depth of the pixel
void main( in InputPS i, out OutputPS o )
{
	//Grey scale depth position
	o.Colour = length(i.ViewPos.xyz) / FarDistance;
}