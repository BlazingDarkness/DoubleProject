struct InputVS
{
	uint VertexIndex : SV_VERTEXID;
};

struct OutputVS
{
	float4 Pos	: SV_POSITION;
};

void main(in InputVS i, out OutputVS o)
{
	//Taken from
	//http://gamedev.stackexchange.com/questions/98283/how-do-i-draw-a-full-screen-quad-in-directx-11
	float2 texcoord = float2(i.VertexIndex & 1, i.VertexIndex >> 1);
	o.Pos = float4((texcoord.x - 0.5f) * 2, -(texcoord.y - 0.5f) * 2, 0, 1);
}