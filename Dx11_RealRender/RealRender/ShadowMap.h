//***************************************************************************************
// ShadowMap.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper class for implementing shadows via shadow mapping algorithm.
//***************************************************************************************

#pragma once
#include "ShareValue.h"

struct BoundingSphere
{
	BoundingSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	XMFLOAT3 Center;
	float Radius;
};

class DeviceDx11;
class ShadowMap
{
public:
	ShadowMap(DeviceDx11* pDevice);
	~ShadowMap();

	ALIGNED16(ShadowMap);

	ID3D11ShaderResourceView* DepthMapSRV();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);
	void SetShadowRasterizerState(ID3D11DeviceContext* dc);

private:
	ShadowMap(const ShadowMap& rhs);
	ShadowMap& operator=(const ShadowMap& rhs);

public:
	void Update(const DirLightBuffer& dirLightInfo);	
	void PrepareShadowMap(ID3D11DeviceContext* dc);
	void AdjustShadowMap(ID3D11DeviceContext* dc);	
	XMMATRIX&	GetLightView();
	XMMATRIX&	GetLightProj();
	XMVECTOR&	GetLightPos();


private:
	UINT m_Width;
	UINT m_Height;

	ID3D11ShaderResourceView* m_DepthMapSRV;
	ID3D11DepthStencilView* m_DepthMapDSV;

	D3D11_VIEWPORT m_Viewport;
	BoundingSphere m_SceneBounds;
	ID3D11RasterizerState* m_ShadowRS;

	XMVECTOR	m_LightPos;
	XMFLOAT4X4 m_LightView;
	XMFLOAT4X4 m_LightProj;
	XMFLOAT4X4 m_ShadowTransform;	
	
	ShadowMapBuffer	m_ShadowMapInfo;
	ID3D11SamplerState* m_ShadowSampler;
	ID3D11Buffer* m_ShadowMapBuffer;
};

