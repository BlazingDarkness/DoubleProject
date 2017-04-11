Texture2D<uint2> LightGrid : register(t0);

static const float colourRange = 15.0f;

struct InputPS
{
	float4 ScreenPos	: SV_POSITION;
};

struct OutputPS
{
	float4 Colour : SV_TARGET;
};

void main(in InputPS i, out OutputPS o)
{
	uint2 Tile = uint2((uint)(i.ScreenPos.x), (uint)(i.ScreenPos.y)) / 16;

	uint lightCount = LightGrid[Tile].y;
	float fLightCount = (float)(LightGrid[Tile].y);

	float4 Colour = float4(0.0f, 0.0f, 0.0f, 1.0f);

	Colour.b = saturate(2.0f - fLightCount / colourRange);

	if(fLightCount < colourRange)
		Colour.g = saturate(fLightCount / colourRange);
	else
		Colour.g = saturate(4.0f - fLightCount / colourRange);

	Colour.r = saturate(fLightCount / colourRange - 2.0f);

	if ((uint)(i.ScreenPos.x) % 16 == 0
		|| (uint)(i.ScreenPos.y) % 16 == 0)
	{
		Colour = float4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Colour.rgb = Colour.rgb * 0.5f; //Not too bright that it hurts your eyes

	o.Colour = Colour;
}