
///////////////////////////
// Types

struct BasicPixelInput
{
	float4 ProjPos		: SV_POSITION;
};


///////////////////////////
// Shader

//Calculates the depth of the pixel
float4 main( BasicPixelInput vIn ) : SV_TARGET
{
	//Grey scale depth position
	return vIn.ProjPos.z / vIn.ProjPos.w;
}