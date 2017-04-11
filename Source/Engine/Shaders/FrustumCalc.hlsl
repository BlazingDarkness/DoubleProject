#define COMPUTER_SHADER
#define GLOBAL_THREAD_DATA b0
#define FRUSTUM_DATA b1
#define GLOBAL_MATRIX b2
#include "CommonStructs.h"

Texture2D DepthBuffer : register(t0);
RWStructuredBuffer<Frustum> FrustumBuffer : register(u0);

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void main(CSInput input)
{
	uint tileX = input.DispatchThreadID.x;
	uint tileY = input.DispatchThreadID.y;

	//Screen Space
	float4 topLeft  = float4((float)tileX		* ((float)TILE_SIZE) / ScreenWidth, (float)tileY       * ((float)TILE_SIZE) / ScreenHeight, -1.0f, 1.0f);
	float4 topRight = float4((float)(tileX + 1) * ((float)TILE_SIZE) / ScreenWidth, (float)tileY       * ((float)TILE_SIZE) / ScreenHeight, -1.0f, 1.0f);
	float4 botLeft  = float4((float)tileX		* ((float)TILE_SIZE) / ScreenWidth, (float)(tileY + 1) * ((float)TILE_SIZE) / ScreenHeight, -1.0f, 1.0f);
	float4 botRight = float4((float)(tileX + 1) * ((float)TILE_SIZE) / ScreenWidth, (float)(tileY + 1) * ((float)TILE_SIZE) / ScreenHeight, -1.0f, 1.0f);
	
	//Clip Space
	topLeft.xy = float2(topLeft.x, 1.0f - topLeft.y) * 2.0f - 1.0f;
	topRight.xy = float2(topRight.x, 1.0f - topRight.y) * 2.0f - 1.0f;
	botLeft.xy = float2(botLeft.x, 1.0f - botLeft.y) * 2.0f - 1.0f;
	botRight.xy = float2(botRight.x, 1.0f - botRight.y) * 2.0f - 1.0f;

	//View Space
	float4 farBottomLeft  = mul(botLeft,  InvProjMatrix);
	float4 farBottomRight = mul(botRight, InvProjMatrix);
	float4 farTopRight    = mul(topRight, InvProjMatrix);
	float4 farTopLeft     = mul(topLeft,  InvProjMatrix);

	//World Space
	farBottomLeft  = mul(farBottomLeft / farBottomLeft.w,  CameraMatrix);
	farBottomRight = mul(farBottomRight / farBottomRight.w, CameraMatrix);
	farTopRight    = mul(farTopRight / farTopRight.w,    CameraMatrix);
	farTopLeft     = mul(farTopLeft / farTopLeft.w,     CameraMatrix);

	Frustum f;

	f.Top.Point = CameraPos.xyz;
	f.Bottom.Point = CameraPos.xyz;
	f.Right.Point = CameraPos.xyz;
	f.Left.Point = CameraPos.xyz;
	f.Far.Point = CameraPos.xyz + CameraForward.xyz * FarDistance;
	f.Near.Point = CameraPos.xyz + CameraForward.xyz * NearDistance;
	
	f.Top.Normal = normalize(cross(((farTopLeft + farTopRight) / 2.0f - CameraPos).xyz, CameraRight.xyz));
	f.Bottom.Normal = normalize(cross(CameraRight.xyz, ((farBottomLeft + farBottomRight) / 2.0f - CameraPos).xyz));
	f.Right.Normal = normalize(cross(CameraUp.xyz, ((farTopRight + farBottomRight) / 2.0f - CameraPos).xyz));
	f.Left.Normal = normalize(cross(((farTopLeft + farBottomLeft) / 2.0f - CameraPos).xyz, CameraUp.xyz));
	f.Far.Normal = CameraForward.xyz;
	f.Near.Normal = -CameraForward.xyz;
	
	//Thread is not gauranteed to be a tile on the screen
	//as the screen is not gauranteed to be to have dimensions in multiples of 256
	if (tileX < NumTileCols && tileY < NumTileRows)
	{
		FrustumBuffer[tileX + tileY * NumTileCols] = f;
	}
}