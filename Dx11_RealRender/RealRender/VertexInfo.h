#pragma once

struct SimpleVertex
{
	XMFLOAT3 Pos;
};

struct BoxVertex
{
	XMFLOAT3 Pos;	
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
};

struct TreeBillboardVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Size;
};

struct SphereVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
	XMFLOAT3 Tangent;
};