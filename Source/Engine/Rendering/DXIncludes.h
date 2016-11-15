#pragma once
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>

#define SAFE_RELEASE(x) if(x != NULL) x->Release(); x = NULL;
#define SAFE_DELETE(x) if(x != NULL) delete x; x = NULL;

#define AL16 __declspec(align(16))