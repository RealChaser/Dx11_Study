#pragma once
#include "stdafx.h"
#include "TemplateUtil.h"
#include "VertexInfo.h"
#include "ConstantBufferInfo.h"

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

struct ObjectResource
{	
	ID3D11Buffer*		pVertexBuffer = nullptr;
	ID3D11Buffer*		pIndexBuffer = nullptr;
	ID3D11Buffer*		pInstnaceBuffer = nullptr;		
	int indexNum;

	virtual ~ObjectResource()
	{
		Safe_Release(pVertexBuffer);
		Safe_Release(pIndexBuffer);	
		Safe_Release(pInstnaceBuffer);			
	}
};

struct InstanceInfo
{
	XMFLOAT4X4 World;
	XMFLOAT4 Color;
};

struct TerrainInfo
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT2 BoundsY;
};

using Func_void_initList = void(*)(initializer_list<string>);	//c++11 using syntax

//std macro
#define STD_FIND_IF(type, func) find_if(type.begin(), type.end(), func)
#define STD_REMOVE_IF(type, func) remove_if(type.begin(), type.end(), func)
#define STD_ERASE(type, iter) type.erase(iter, type.end())

#define ALIGNED16(OBJECT)\
public:\
	void* operator new(size_t size) {	return (OBJECT*) _aligned_malloc(sizeof(OBJECT), 16); } \
	void operator delete(void* ptr) { _aligned_free(ptr); }

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#ifdef CONSOLE
	#define CONSOLE_PRINT(x) printf(x);
#else
	#define CONSOLE_PRINT(x)
#endif

//Math
const float PI = 3.1415926535f;
const float MAPINFINITY = 3.402823466e+38F;
const float EPSILON = 1e-20f;

class RenderInfo
{
public:
	UINT		stride = 0;
	UINT		offset = 0;
	UINT		visibleObjectCount = 0;
	D3D_PRIMITIVE_TOPOLOGY	primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

class ConstantInfo
{
public:
	XMMATRIX	world;
	XMFLOAT4	color = { 0, 0, 0, 1};
	Material	material;
	UINT heightmapWidth;
	UINT heightmapHeight;
	float cellSpacing;
	ALIGNED16(ConstantInfo);
};

enum RENDER_KEY{ SOLID, LIGHT, TEXTURE, REFLECT, DISPLACEMENT, SKY, TERRAIN, INSTANCE, BILLBOARD, END };