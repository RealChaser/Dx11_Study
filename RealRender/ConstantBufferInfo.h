#pragma once

struct VPBuffer
{
	XMMATRIX ViewProj;
};

struct WVPBuffer
{
	XMMATRIX WorldViewProj;
};

struct ColorBuffer
{
	XMFLOAT4 Color;
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
};

struct VPMPBuffer
{
	XMMATRIX ViewProj;
	Material Material;
	XMFLOAT3 EyePosW;
	float pad;
};

struct WVPMBuffer
{
	XMMATRIX World;
	XMMATRIX WorldViewProj;
	Material Material;
};

struct DisplacementValueBuffer
{
	XMFLOAT4 TessValue;		//(gMaxTessDistance, gMinTessDistance, gMaxTessFactor, gMinTessFactor);
	XMFLOAT4 gHeightScale;	//(value, x, x, x) value만 사용한다 x는 pad;
	XMFLOAT3 EyePos;
	float pad;
};

struct WVPTtMBuffer
{
	XMMATRIX World;
	XMMATRIX ViewProj;
	XMMATRIX TexTransform;
	Material Material;
};

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad;
};

struct DirLightBuffer
{
	DirectionalLight mDirLight[3];
	XMFLOAT3 mEyePosW;
	float Pad;
};

struct TerrainTSBuffer
{
	XMMATRIX ViewProj;
	XMFLOAT4 DistTess;
	XMFLOAT4 TexScale;
	XMFLOAT4 WorldFrustumPlanes[6];
	XMFLOAT3 EyePos;
	float pad;
};

struct TerrainPSBuffer
{
	XMFLOAT4 FogInfo;
	XMFLOAT4 FogColor;
	XMFLOAT4 TexelCellInfo;
	Material gMaterial;
};