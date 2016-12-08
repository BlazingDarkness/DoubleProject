
///////////////////////////
// Types

struct InputPS
{
	float4 ProjPos	: SV_POSITION;
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
	o.Colour = clamp(i.ProjPos.z / i.ProjPos.w, 0.0f, 1.0f);
}