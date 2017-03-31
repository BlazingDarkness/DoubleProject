#define COMPUTER_SHADER
#define GLOBAL_THREAD_DATA b0
#define FRUSTUM_DATA b1
#include "CommonStructs.h"

RWStructuredBuffer<Frustum> FrustumBuffer : register(u0);

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void main(CSInput input)
{
	float farHeightHalf = tan(FOV / 2.0f) * FarDistance;
	float farWidthHalf = farHeight * ScreenWidth / ScreenHeight;

	float farTileHeight = farHeightHalf * ((float)TILE_SIZE) / ScreenHeight;
	float farTileWidth = farWidthHalf * ((float)TILE_SIZE) / ScreenWidth;

	uint tileX = input.DispatchThreadID.x;
	uint tileY = input.DispatchThreadID.y;

	float farCenter = CameraPos + CameraForward * FarDistance;

	float3 farBottomLeft = farCenter - (CameraUp * farHeightHalf) - (CameraRight * farWidthHalf);
	farBottomLeft = farBottomLeft + (CameraRight * (float)tileX * farTileWidth) + (CameraUp * (float)tileY * farTileHeight);

	float3 farTopLeft = farBottomLeft + (CameraUp * farTileHeight);
	float3 farTopRight = farTopLeft + (CameraRight * farTileWidth);
	float3 farBottomRight = farBottomLeft + (CameraRight * farTileWidth);

	Frustum f;

	f.Top.Point = CameraPos;
	f.Bottom.Point = CameraPos;
	f.Right.Point = CameraPos;
	f.Left.Point = CameraPos;
	f.Far.Point = farCenter;
	f.Near.Point = CameraPos + CameraForward * NearDistance;
	
	f.Top.Normal = normalize(cross(tp - cp, CameraRight));
	f.Bottom.Normal = normalize(cross(CameraRight, bp - cp));
	f.Right.Normal = normalize(cross(CameraUp, rp - cp));
	f.Left.Normal = normalize(cross(lp - cp, CameraUp));
	f.Far.Normal = CameraForward;
	f.Near.Normal = -CameraForward;

	
	//fPlanes[0].Point = float3(0.0f, 0.0f, 0.0f);
	//fPlanes[0].Normal = 

}