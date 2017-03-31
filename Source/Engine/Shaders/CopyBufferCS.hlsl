#define COMPUTER_SHADER
#define GLOBAL_THREAD_DATA b0
#define COPY_DETAILS b1
#include "CommonStructs.h"

StructuredBuffer<uint> source : register(t0);
RWStructuredBuffer<uint> destination : register(u0);

[numthreads(2, 2, 1)]
void main(CSInput input)
{
	uint increment = 2 * 2;
	for (uint i = input.GroupIndex; i < MinSize; i += increment)
	{
		destination[i] = source[i];
	}
}