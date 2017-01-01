#ifdef __cplusplus
#pragma once
#include "CMatrix4x4.h"
#define SEMANTIC(sem) 
#define CBUFFER struct
#define ROW_MAJOR
#define UINT unsigned int
#define Mat4 gen::CMatrix4x4
#define Vec4 gen::CVector4
#define Vec3 gen::CVector3
#define Vec2 gen::CVector2
#define GLOBAL_MATRIX b0
#define OBJECT_MATRIX b1
#define MATERIAL_DATA b1
#define GLOBAL_LIGHT_DATA b0
#define GLOBAL_THREAD_DATA b1
#else
#define SEMANTIC(sem) sem
#define CBUFFER cbuffer
#define ROW_MAJOR row_major
#define UINT uint
#define Mat4 float4x4
#define Vec4 float4
#define Vec3 float3
#define Vec2 float2
#endif

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
	UINT NumOfLights	SEMANTIC(: packoffset(c2.y));
	Vec2 Padding		SEMANTIC(: packoffset(c2.z));
};
#endif

#ifdef GLOBAL_THREAD_DATA
CBUFFER GlobalThreadData SEMANTIC(: register(GLOBAL_THREAD_DATA))
{
	Vec4 NumOfThreads		SEMANTIC(: packoffset(c0));
	Vec4 NumOfThreadGroups	SEMANTIC(: packoffset(c1));
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

struct Light
{
	Vec3 Position;
	float Brightness;
	Vec3 Colour;
	float Range;
};