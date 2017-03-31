#ifdef __cplusplus
#pragma once
#include "CMatrix4x4.h"
#define SEMANTIC(sem) 
#define CBUFFER struct
#define ROW_MAJOR
#define UINT unsigned int
#define Mat4 gen::CMatrix4x4
#define Vec4Int(name) unsigned int name[4]
#define Vec4 gen::CVector4
#define Vec3 gen::CVector3
#define Vec2 gen::CVector2

#define GLOBAL_MATRIX b0
#define OBJECT_MATRIX b1
#define MATERIAL_DATA b1
#define GLOBAL_LIGHT_DATA b1
#define GLOBAL_THREAD_DATA b0
#define COPY_DETAILS b1
#define FRUSTUM_DATA b1
#else
#define SEMANTIC(sem) sem
#define CBUFFER cbuffer
#define ROW_MAJOR row_major
#define UINT uint
#define Mat4 float4x4
#define Vec4Int(name) uint4 name
#define Vec4 float4
#define Vec3 float3
#define Vec2 float2
#endif

static const UINT TILE_SIZE = 16;

#ifdef GLOBAL_MATRIX
CBUFFER GlobalMatrix SEMANTIC(: register(GLOBAL_MATRIX))
{
	ROW_MAJOR Mat4 ViewMatrix;
	ROW_MAJOR Mat4 ProjMatrix;
};
#endif

#ifdef OBJECT_MATRIX
CBUFFER ObjectMatrix SEMANTIC(: register(OBJECT_MATRIX))
{
	ROW_MAJOR Mat4 WorldMatrix;
};
#endif

#ifdef GLOBAL_LIGHT_DATA
CBUFFER GlobalLightData SEMANTIC(: register(GLOBAL_LIGHT_DATA))
{
	Vec4 AmbientColour	SEMANTIC(: packoffset(c0));
	Vec4 CameraPos		SEMANTIC(: packoffset(c1));
	float SpecularPower	SEMANTIC(: packoffset(c2));
	float ScreenWidth	SEMANTIC(: packoffset(c2.y));
	float ScreenHeight	SEMANTIC(: packoffset(c2.z));
	UINT NumOfLights	SEMANTIC(: packoffset(c2.w));
};
#endif

#ifdef GLOBAL_THREAD_DATA
CBUFFER GlobalThreadData SEMANTIC(: register(GLOBAL_THREAD_DATA))
{
	Vec4Int(NumOfThreads)		SEMANTIC(: packoffset(c0));
	Vec4Int(NumOfThreadGroups)	SEMANTIC(: packoffset(c1));
};
#endif

#ifdef MATERIAL_DATA
CBUFFER MaterialData SEMANTIC(: register(MATERIAL_DATA))
{
	Vec4 DiffuseColour		SEMANTIC(: packoffset(c0));
	float Alpha				SEMANTIC(: packoffset(c1));
	float Dirtyness			SEMANTIC(: packoffset(c1.y));
	float Shinyness			SEMANTIC(: packoffset(c1.z));
	UINT HasAlpha			SEMANTIC(: packoffset(c1.w));
	UINT HasDirt			SEMANTIC(: packoffset(c2));
	UINT HasDiffuseTex		SEMANTIC(: packoffset(c2.y));
	UINT HasSpecTex			SEMANTIC(: packoffset(c2.z));
	float MaterialPadding	SEMANTIC(: packoffset(c2.w));
};
#endif

#ifdef COPY_DETAILS
CBUFFER CopyDetails SEMANTIC(: register(COPY_DETAILS))
{
	UINT SourceSize			SEMANTIC(: packoffset(c0));
	UINT DestinationSize	SEMANTIC(: packoffset(c0.y));
	UINT MinSize			SEMANTIC(: packoffset(c0.z));
	UINT CopyPadding		SEMANTIC(: packoffset(c0.w));
};
#endif

#ifdef FRUSTUM_DATA
CBUFFER FrustumData SEMANTIC(: register(CAMERA_DATA))
{
	Vec4 CameraRight		SEMANTIC(: packoffset(c0));
	Vec4 CameraUp			SEMANTIC(: packoffset(c1));
	Vec4 CameraForward		SEMANTIC(: packoffset(c2));
	Vec4 CameraPos			SEMANTIC(: packoffset(c3));
	float FOV				SEMANTIC(: packoffset(c4));
	float FarDistance		SEMANTIC(: packoffset(c4.y));
	float NearDistance		SEMANTIC(: packoffset(c4.z));
	float Ratio				SEMANTIC(: packoffset(c4.w));
	float ScreenWidth		SEMANTIC(: packoffset(c5));
	float ScreenHeight		SEMANTIC(: packoffset(c5.y));
	UINT NumTileRows		SEMANTIC(: packoffset(c5.z));
	UINT NumTileCols		SEMANTIC(: packoffset(c5.w));
};
#endif

struct Light
{
	Vec3 Position;
	float Brightness;
	Vec3 Colour;
	float Range;
};

struct Plane
{
	Vec3 Point;
	Vec3 Normal;
	float Padding[2];
};

struct Frustum
{
	Plane Top;
	Plane Bottom;
	Plane Right;
	Plane Left;
	Plane Near;
	Plane Far;
};

struct Sphere
{
	Vec3 Position;
	float Radius;
};

#ifdef COMPUTER_SHADER
//Struct taken from 3dgep.com
struct CSInput
{
	uint3 GroupID : SV_GroupID;           // 3D index of the thread group in the dispatch.
	uint3 GroupThreadID : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
	uint3 DispatchThreadID : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
	uint  GroupIndex : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};
#endif