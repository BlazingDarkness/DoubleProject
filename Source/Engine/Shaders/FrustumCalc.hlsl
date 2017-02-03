#define COMPUTER_SHADER
#define GLOBAL_THREAD_DATA b0
#include "CommonStructs.h"

static const uint TILE_SIZE = 16;

///////////////////////////
// Globals/Constant Buffers

///////////////////////////
// Types






[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

}