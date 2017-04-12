//***************************************************************************************
// ShadowMap.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#include "stdafx.h"
#include "ShadowMap.h"
#include "DeviceDx11.h"

#define SHADOWMAP_SIZE 2048
ShadowMap::ShadowMap(DeviceDx11* pDevice)
	: m_DepthMapSRV(nullptr), m_DepthMapDSV(nullptr), m_ShadowRS(nullptr)
{	
	m_Width = SHADOWMAP_SIZE;
	m_Height = SHADOWMAP_SIZE;
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_Viewport.Width = static_cast<float>(SHADOWMAP_SIZE);
	m_Viewport.Height = static_cast<float>(SHADOWMAP_SIZE);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
	// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = m_Width;
	texDesc.Height = m_Height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = 0;
	pDevice->GetDevice()->CreateTexture2D(&texDesc, 0, &depthMap);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	pDevice->GetDevice()->CreateDepthStencilView(depthMap, &dsvDesc, &m_DepthMapDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	pDevice->GetDevice()->CreateShaderResourceView(depthMap, &srvDesc, &m_DepthMapSRV);

	// View saves a reference to the texture so we can release our reference.	
	Safe_Release(depthMap);

	m_SceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_SceneBounds.Radius = sqrtf(10.0f*10.0f + 15.0f*15.0f);

	// [From MSDN]
	// If the depth buffer currently bound to the output-merger stage has a UNORM format or
	// no depth buffer is bound the bias value is calculated like this: 
	//
	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	//
	// where r is the minimum representable value > 0 in the depth-buffer format converted to float32.
	// [/End MSDN]
	// 
	// For a 24-bit depth buffer, r = 1 / 2^24.
	//
	// Example: DepthBias = 100000 ==> Actual DepthBias = 100000/2^24 = .006

	// You need to experiment with these values for your scene.
	D3D11_RASTERIZER_DESC shadowRD;
	ZeroMemory(&shadowRD, sizeof(D3D11_RASTERIZER_DESC));
	shadowRD.DepthBias = 10000;
	shadowRD.DepthBiasClamp = 0.0f;
	shadowRD.SlopeScaledDepthBias = 1.0f;	
	pDevice->GetDevice()->CreateRasterizerState(&shadowRD, &m_ShadowRS);

	//------------------
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ShadowMapBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HRESULT hr = DeviceDx11::GetInstance().GetDevice()->CreateBuffer(&bd, NULL, &m_ShadowMapBuffer);

	//---------------------
	D3D11_SAMPLER_DESC shadowSample;
	ZeroMemory(&shadowSample, sizeof(shadowSample));

	shadowSample.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	shadowSample.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSample.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSample.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSample.ComparisonFunc = D3D11_COMPARISON_LESS;	
	pDevice->GetDevice()->CreateSamplerState(&shadowSample, &m_ShadowSampler);

}

ShadowMap::~ShadowMap()
{
	Safe_Release(m_DepthMapSRV);
	Safe_Release(m_DepthMapDSV);	
}

ID3D11ShaderResourceView* ShadowMap::DepthMapSRV()
{
	return m_DepthMapSRV;
}

void ShadowMap::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(1, &m_Viewport);

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	dc->OMSetRenderTargets(1, renderTargets, m_DepthMapDSV);

	dc->ClearDepthStencilView(m_DepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);	
}

void ShadowMap::Update(const DirLightBuffer& dirLightInfo)
{		
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&dirLightInfo.m_DirLight[0].Direction);
	XMVECTOR lightPos = -2.0f * m_SceneBounds.Radius * lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&m_SceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - m_SceneBounds.Radius;
	float b = sphereCenterLS.y - m_SceneBounds.Radius;
	float n = sphereCenterLS.z - m_SceneBounds.Radius;
	float r = sphereCenterLS.x + m_SceneBounds.Radius;
	float t = sphereCenterLS.y + m_SceneBounds.Radius;
	float f = sphereCenterLS.z + m_SceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&m_LightView, V);
	XMStoreFloat4x4(&m_LightProj, P);
	XMStoreFloat4x4(&m_ShadowTransform, S);

	m_LightPos = lightPos;
}

void ShadowMap::SetShadowRasterizerState(ID3D11DeviceContext* dc)
{
	dc->RSSetState(m_ShadowRS);
}

void ShadowMap::PrepareShadowMap(ID3D11DeviceContext* dc)
{	//그림자 맵!!을 그리기 위한 값들을 전달
	m_ShadowMapInfo.useShadowMap = { 0, 0, 0, 0 };
	m_ShadowMapInfo.shadowTransform = XMLoadFloat4x4(&m_ShadowTransform);	//값은 전달하지만 사용하진 않는다..으..
	dc->UpdateSubresource(m_ShadowMapBuffer, 0, NULL, &m_ShadowMapInfo, 0, 0);
	
	dc->PSSetConstantBuffers(1, 1, &m_ShadowMapBuffer);
	dc->PSSetShaderResources(0, 1, &m_DepthMapSRV);
	dc->PSSetSamplers(0, 1, &m_ShadowSampler);
}

void ShadowMap::AdjustShadowMap(ID3D11DeviceContext* dc)
{	//그림자!!를 그리기 위한 값들을 전달
	m_ShadowMapInfo.useShadowMap = { 1, 0, 0, 0 };
	m_ShadowMapInfo.shadowTransform = XMLoadFloat4x4(&m_ShadowTransform);
	dc->UpdateSubresource(m_ShadowMapBuffer, 0, NULL, &m_ShadowMapInfo, 0, 0);
	
	dc->PSSetShaderResources(0, 1, &m_DepthMapSRV);	//그려진 그림자 맵을 쉐이더에 전달
	dc->PSSetConstantBuffers(1, 1, &m_ShadowMapBuffer);
}

XMMATRIX& ShadowMap::GetLightView()
{
	return XMLoadFloat4x4(&m_LightView);
}

XMMATRIX& ShadowMap::GetLightProj()
{
	return XMLoadFloat4x4(&m_LightProj);
}

XMVECTOR& ShadowMap::GetLightPos()
{
	return m_LightPos;
}
