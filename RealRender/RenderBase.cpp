#include "stdafx.h"
#include "RenderBase.h"
#include "ResourceManager.h"
#include "DeviceDx11.h"
#include "ShaderUtil.h"
#include "Camera.h"

DeviceDx11* RenderBase::wrapDevice = nullptr;

RenderBase::RenderBase()
: m_pVertexLayout(nullptr),
m_pVertexShader(nullptr),
m_pGeometryShader(nullptr),
m_pPixelShader(nullptr),
m_pHullShader(nullptr),
m_pDomainShader(nullptr),
m_Renderkey(RENDER_KEY::END)
{
}

RenderBase::~RenderBase()
{	
	Safe_Release_VecList(m_vecTextureSRV);
	Safe_Release_Map(m_mapConstant);
	Safe_Release_Map(m_mapSampler);
	Safe_Release(m_pVertexLayout);
	Safe_Release(m_pVertexShader);
	Safe_Release(m_pGeometryShader);
	Safe_Release(m_pPixelShader);
	Safe_Release(m_pHullShader);
	Safe_Release(m_pDomainShader);
}

void RenderBase::InitConstant()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WVPMBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "WVPMBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["WVPMBuffer"]);
}

//-------------------------------------------------------------------
SolidShaderRender::SolidShaderRender()
{
	m_ShaderName = L"SolidShader.fx";
	m_Renderkey = RENDER_KEY::SOLID;
}

void SolidShaderRender::InitShader()
{
	D3D11_INPUT_ELEMENT_DESC LAYOUT_P[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(LAYOUT_P);

	ID3DBlob* pVSBlob = nullptr;
	ShaderUtil::CreateVertexShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "VS", "vs_5_0", &pVSBlob, &m_pVertexShader);
	wrapDevice->GetDevice()->CreateInputLayout(LAYOUT_P, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout);
	Safe_Release(pVSBlob);

	ID3DBlob* pPSBlob = nullptr;
	ShaderUtil::CreatePixelShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "PS", "ps_5_0", &pPSBlob, &m_pPixelShader);
	Safe_Release(pPSBlob);
}

void SolidShaderRender::SetupConstant(const ConstantInfo* info)
{
	Camera& camera = Camera::GetInstance();	

	WVPBuffer wvp_cb;
	wvp_cb.WorldViewProj = XMMatrixTranspose(info->world * camera.View() * camera.Proj());
	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["WVPBuffer"], 0, NULL, &wvp_cb, 0, 0);

	ColorBuffer color_cb;
	color_cb.Color = info->color;
	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["ColorBuffer"], 0, NULL, &color_cb, 0, 0);
}

void SolidShaderRender::Render(ObjectResource* pResource, const RenderInfo* info)
{
	ID3D11DeviceContext* context = wrapDevice->GetDeviceContext();
	context->IASetInputLayout(m_pVertexLayout);
	context->IASetVertexBuffers(0, 1, &pResource->pVertexBuffer, &info->stride, &info->offset);
	context->IASetIndexBuffer(pResource->pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(info->primitive_topology);

	context->VSSetConstantBuffers(1, 1, &m_mapConstant["WVPBuffer"]);
	context->PSSetConstantBuffers(2, 1, &m_mapConstant["ColorBuffer"]);
	context->VSSetShader(m_pVertexShader, NULL, 0);
	context->PSSetShader(m_pPixelShader, NULL, 0);
	context->DrawIndexed(pResource->indexNum, 0, 0);
}

void SolidShaderRender::InitConstant()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WVPBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "WVPBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["WVPBuffer"]);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ColorBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "ColorBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["ColorBuffer"]);
}

//--------------------------------------------------------

LightShaderRender::LightShaderRender()
{
	m_ShaderName = L"LightShader.fx";
	m_Renderkey = RENDER_KEY::LIGHT;
}

void LightShaderRender::InitShader()
{
	D3D11_INPUT_ELEMENT_DESC LAYOUT_PN[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(LAYOUT_PN);	

	ID3DBlob* pVSBlob = nullptr;
	ShaderUtil::CreateVertexShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "VS", "vs_5_0", &pVSBlob, &m_pVertexShader);
	wrapDevice->GetDevice()->CreateInputLayout(LAYOUT_PN, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout);
	Safe_Release(pVSBlob);

	ID3DBlob* pPSBlob = nullptr;
	ShaderUtil::CreatePixelShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "PS", "ps_5_0", &pPSBlob, &m_pPixelShader);
	Safe_Release(pPSBlob);
}

void LightShaderRender::SetupConstant(const ConstantInfo* info)
{
	Camera& camera = Camera::GetInstance();

	WVPMBuffer cb;
	cb.World = XMMatrixTranspose(info->world);
	cb.WorldViewProj = XMMatrixTranspose(info->world * camera.View() * camera.Proj());
	cb.Material = info->material;
	
	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["WVPMBuffer"], 0, NULL, &cb, 0, 0);
}

void LightShaderRender::Render(ObjectResource* pResource, const RenderInfo* info)
{
	ID3D11DeviceContext* context = wrapDevice->GetDeviceContext();
	context->IASetInputLayout(m_pVertexLayout);
	context->IASetVertexBuffers(0, 1, &pResource->pVertexBuffer, &info->stride, &info->offset);
	context->IASetIndexBuffer(pResource->pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(info->primitive_topology);

	context->VSSetConstantBuffers(1, 1, &m_mapConstant["WVPMBuffer"]);
	context->PSSetConstantBuffers(1, 1, &m_mapConstant["WVPMBuffer"]);
	context->VSSetShader(m_pVertexShader, NULL, 0);
	context->PSSetShader(m_pPixelShader, NULL, 0);

	context->DrawIndexed(pResource->indexNum, 0, 0);
}

void LightShaderRender::InitConstant()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WVPMBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "WVPMBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["WVPMBuffer"]);
}

//---------
TextureShaderRender::TextureShaderRender()
{
	m_ShaderName = L"TextureShader.fx";
	m_Renderkey = RENDER_KEY::TEXTURE;
}

void TextureShaderRender::InitShader()
{
	D3D11_INPUT_ELEMENT_DESC LAYOUT_PNT[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};	

	UINT numElements = ARRAYSIZE(LAYOUT_PNT);

	ID3DBlob* pVSBlob = nullptr;
	ShaderUtil::CreateVertexShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "VS", "vs_5_0", &pVSBlob, &m_pVertexShader);
	wrapDevice->GetDevice()->CreateInputLayout(LAYOUT_PNT, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout);
	Safe_Release(pVSBlob);

	ID3DBlob* pPSBlob = nullptr;
	ShaderUtil::CreatePixelShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "PS", "ps_5_0", &pPSBlob, &m_pPixelShader);
	Safe_Release(pPSBlob);
}

void TextureShaderRender::InitSampler()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	m_mapSampler.insert({ "SampleLinear", nullptr });
	wrapDevice->GetDevice()->CreateSamplerState(&sampDesc, &m_mapSampler["SampleLinear"]);
}

void TextureShaderRender::SetupConstant(const ConstantInfo* info)
{
	Camera& camera = Camera::GetInstance();		

	WVPBuffer cb;
	cb.WorldViewProj = XMMatrixTranspose(info->world * camera.View() * camera.Proj());

	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["WVPBuffer"], 0, NULL, &cb, 0, 0);
}

void TextureShaderRender::Render(ObjectResource* pResource, const RenderInfo* info)
{
	ID3D11DeviceContext* context = wrapDevice->GetDeviceContext();
	context->IASetInputLayout(m_pVertexLayout);
	context->IASetVertexBuffers(0, 1, &pResource->pVertexBuffer, &info->stride, &info->offset);
	context->IASetIndexBuffer(pResource->pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(info->primitive_topology);

	context->VSSetConstantBuffers(1, 1, &m_mapConstant["WVPBuffer"]);
	context->VSSetShader(m_pVertexShader, NULL, 0);
	context->PSSetShader(m_pPixelShader, NULL, 0);
	context->PSSetShaderResources(0, 1, &m_vecTextureSRV[0] );
	context->PSSetSamplers(0, 1, &m_mapSampler["SampleLinear"]);
	context->DrawIndexed(pResource->indexNum, 0, 0);
}

void TextureShaderRender::InitConstant()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WVPBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "WVPBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["WVPBuffer"]);
}

ReflectShaderRender::ReflectShaderRender()
{
	m_ShaderName = L"ReflectShader.fx";
	m_Renderkey = RENDER_KEY::REFLECT;
}

void ReflectShaderRender::InitShader()
{
	D3D11_INPUT_ELEMENT_DESC LAYOUT_P0C0[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(LAYOUT_P0C0);

	ID3DBlob* pVSBlob = nullptr;
	ShaderUtil::CreateVertexShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "VS", "vs_5_0", &pVSBlob, &m_pVertexShader);
	wrapDevice->GetDevice()->CreateInputLayout(LAYOUT_P0C0, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout);
	Safe_Release(pVSBlob);

	ID3DBlob* pPSBlob = nullptr;
	ShaderUtil::CreatePixelShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "PS", "ps_5_0", &pPSBlob, &m_pPixelShader);
	Safe_Release(pPSBlob);
}

void ReflectShaderRender::InitSampler()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampDesc.MaxAnisotropy = 4;

	m_mapSampler.insert({ "SampleAnisortopic", nullptr });
	wrapDevice->GetDevice()->CreateSamplerState(&sampDesc, &m_mapSampler["SampleAnisortopic"]);
}

void ReflectShaderRender::SetupConstant(const ConstantInfo* info)
{
	Camera& camera = Camera::GetInstance();	

	WVPMBuffer cb;
	cb.World = XMMatrixTranspose(info->world);
	cb.WorldViewProj = XMMatrixTranspose(info->world * camera.View() * camera.Proj());
	cb.Material = info->material;

	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["WVPMBuffer"], 0, NULL, &cb, 0, 0);
}

void ReflectShaderRender::Render(ObjectResource* pResource, const RenderInfo* info)
{
	ID3D11DeviceContext* context = wrapDevice->GetDeviceContext();
	context->IASetInputLayout(m_pVertexLayout);
	context->IASetVertexBuffers(0, 1, &pResource->pVertexBuffer, &info->stride, &info->offset);
	context->IASetIndexBuffer(pResource->pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(info->primitive_topology);

	context->VSSetConstantBuffers(1, 1, &m_mapConstant["WVPMBuffer"]);
	context->PSSetConstantBuffers(1, 1, &m_mapConstant["WVPMBuffer"]);
	context->VSSetShader(m_pVertexShader, NULL, 0);
	context->PSSetShader(m_pPixelShader, NULL, 0);
	context->PSSetShaderResources(0, 1, &m_vecTextureSRV[0]);
	context->PSSetSamplers(0, 1, &m_mapSampler["SampleAnisortopic"]);
	context->DrawIndexed(pResource->indexNum, 0, 0);
}

void ReflectShaderRender::InitConstant()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WVPMBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "WVPMBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["WVPMBuffer"]);
}

DisplacementShaderRender::DisplacementShaderRender()
{					
	m_ShaderName = L"DisplacementMap.fx";
	m_Renderkey = RENDER_KEY::DISPLACEMENT;
}

void DisplacementShaderRender::InitShader()
{
	D3D11_INPUT_ELEMENT_DESC LAYOUT_PNTT[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};	

	UINT numElements = ARRAYSIZE(LAYOUT_PNTT);

	ID3DBlob* pVSBlob = nullptr;
	ShaderUtil::CreateVertexShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "VS", "vs_5_0", &pVSBlob, &m_pVertexShader);
	wrapDevice->GetDevice()->CreateInputLayout(LAYOUT_PNTT, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout);
	Safe_Release(pVSBlob);

	ID3DBlob* pHSBlob = nullptr;
	ShaderUtil::CreateHullShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "HS", "hs_5_0", &pHSBlob, &m_pHullShader);
	Safe_Release(pHSBlob);

	ID3DBlob* pDSBlob = nullptr;
	ShaderUtil::CreateDomainShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "DS", "ds_5_0", &pDSBlob, &m_pDomainShader);
	Safe_Release(pDSBlob);

	ID3DBlob* pPSBlob = nullptr;
	ShaderUtil::CreatePixelShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "PS", "ps_5_0", &pPSBlob, &m_pPixelShader);
	Safe_Release(pPSBlob);
}

void DisplacementShaderRender::InitSampler()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	m_mapSampler.insert({ "SampleLinear", nullptr });
	wrapDevice->GetDevice()->CreateSamplerState(&sampDesc, &m_mapSampler["SampleLinear"]);
}

void DisplacementShaderRender::SetupConstant(const ConstantInfo* info)
{
	Camera& camera = Camera::GetInstance();	

	DisplacementValueBuffer cb;
	cb.TessValue = { 33.0f, 4.0f, 10.0f, 1.0f };
	cb.gHeightScale = { 0.87f, 0, 0, 0 };
	cb.EyePos = camera.GetPosition();

	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["DisplacementValueBuffer"], 0, NULL, &cb, 0, 0);

	WVPTtMBuffer cb2;
	cb2.World = XMMatrixTranspose(info->world);
	cb2.ViewProj = XMMatrixTranspose(camera.View() * camera.Proj());
	cb2.TexTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	cb2.Material = info->material;

	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["WVPTtMBuffer"], 0, NULL, &cb2, 0, 0);
}

void DisplacementShaderRender::Render(ObjectResource* pResource, const RenderInfo* info)
{	
	ID3D11DeviceContext* context = wrapDevice->GetDeviceContext();

	context->IASetInputLayout(m_pVertexLayout);
	context->IASetVertexBuffers(0, 1, &pResource->pVertexBuffer, &info->stride, &info->offset);
	context->IASetIndexBuffer(pResource->pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(info->primitive_topology);

	context->VSSetConstantBuffers(1, 1, &m_mapConstant["DisplacementValueBuffer"]);
	context->DSSetConstantBuffers(1, 1, &m_mapConstant["DisplacementValueBuffer"]);
	context->PSSetConstantBuffers(1, 1, &m_mapConstant["DisplacementValueBuffer"]);
	context->VSSetConstantBuffers(2, 1, &m_mapConstant["WVPTtMBuffer"]);
	context->DSSetConstantBuffers(2, 1, &m_mapConstant["WVPTtMBuffer"]);
	context->PSSetConstantBuffers(2, 1, &m_mapConstant["WVPTtMBuffer"]);
	context->VSSetShader(m_pVertexShader, NULL, 0);
	context->HSSetShader(m_pHullShader, NULL, 0);
	context->DSSetShader(m_pDomainShader, NULL, 0);
	context->PSSetShader(m_pPixelShader, NULL, 0);

	context->PSSetShaderResources(0, 1, &m_vecTextureSRV[0]);
	context->PSSetShaderResources(1, 1, &m_vecTextureSRV[1]);
	context->DSSetShaderResources(1, 1, &m_vecTextureSRV[1]);
	context->DSSetSamplers(0, 1, &m_mapSampler["SampleLinear"]);
	context->PSSetSamplers(0, 1, &m_mapSampler["SampleLinear"]);

	context->DrawIndexed(pResource->indexNum, 0, 0);

	context->HSSetShader(0, 0, 0);
	context->DSSetShader(0, 0, 0);
}

void DisplacementShaderRender::InitConstant()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DisplacementValueBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	m_mapConstant.insert({ "DisplacementValueBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["DisplacementValueBuffer"]);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WVPTtMBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	m_mapConstant.insert({ "WVPTtMBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["WVPTtMBuffer"]);
}

//-------------------
SkyShaderRender::SkyShaderRender()
{
	m_ShaderName = L"Sky.fx";
	m_Renderkey = RENDER_KEY::SKY;
}

SkyShaderRender::~SkyShaderRender()
{
	Safe_Release(m_SkyRS);
	Safe_Release(m_SkyDSS);
}


void SkyShaderRender::InitShader()
{
	D3D11_INPUT_ELEMENT_DESC LAYOUT_P0[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(LAYOUT_P0);	

	ID3DBlob* pVSBlob = nullptr;
	ShaderUtil::CreateVertexShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "VS", "vs_5_0", &pVSBlob, &m_pVertexShader);
	wrapDevice->GetDevice()->CreateInputLayout(LAYOUT_P0, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout);
	Safe_Release(pVSBlob);

	ID3DBlob* pPSBlob = nullptr;
	ShaderUtil::CreatePixelShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "PS", "ps_5_0", &pPSBlob, &m_pPixelShader);
	Safe_Release(pPSBlob);
}

void SkyShaderRender::InitSampler()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	m_mapSampler.insert({ "SampleLinear", nullptr });
	wrapDevice->GetDevice()->CreateSamplerState(&sampDesc, &m_mapSampler["SampleLinear"]);
}

void SkyShaderRender::InitEtc()
{
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;

	wrapDevice->GetDevice()->CreateRasterizerState(&noCullDesc, &m_SkyRS);

	D3D11_DEPTH_STENCIL_DESC DSS_Desc;
	DSS_Desc.DepthEnable = true;
	DSS_Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSS_Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	DSS_Desc.StencilEnable = false;
	DSS_Desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DSS_Desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	DSS_Desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_Desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_Desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSS_Desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	DSS_Desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_Desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DSS_Desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSS_Desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	wrapDevice->GetDevice()->CreateDepthStencilState(&DSS_Desc, &m_SkyDSS);
}

void SkyShaderRender::SetupConstant(const ConstantInfo* info)
{
	Camera& camera = Camera::GetInstance();	

	WVPBuffer cb;
	cb.WorldViewProj = XMMatrixTranspose(info->world * camera.View() * camera.Proj());
	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["WVPBuffer"], 0, NULL, &cb, 0, 0);
}

void SkyShaderRender::Render(ObjectResource* pResource, const RenderInfo* info)
{
	ID3D11DeviceContext* context = wrapDevice->GetDeviceContext();

	context->IASetVertexBuffers(0, 1, &pResource->pVertexBuffer, &info->stride, &info->offset);
	context->IASetIndexBuffer(pResource->pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetInputLayout(m_pVertexLayout);
	context->IASetPrimitiveTopology(info->primitive_topology);
	context->RSSetState(m_SkyRS);
	context->OMSetDepthStencilState(m_SkyDSS, 0);

	context->VSSetConstantBuffers(1, 1, &m_mapConstant["WVPBuffer"]);
	context->VSSetShader(m_pVertexShader, NULL, 0);
	context->PSSetShader(m_pPixelShader, NULL, 0);
	context->PSSetShaderResources(0, 1, &m_vecTextureSRV[0]);
	context->PSSetSamplers(0, 1, &m_mapSampler["SampleLinear"]);
	context->DrawIndexed(pResource->indexNum, 0, 0);
}

void SkyShaderRender::InitConstant()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WVPBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "WVPBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["WVPBuffer"]);
}

//--------------------------------

TerrainShaderRender::TerrainShaderRender()
{
	m_ShaderName = L"Terrain.fx";
	m_Renderkey = RENDER_KEY::TERRAIN;
}

void TerrainShaderRender::InitShader()
{
	D3D11_INPUT_ELEMENT_DESC LAYOUT_P0T0T1[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(LAYOUT_P0T0T1);

	m_ShaderName.c_str();
	wstring shaderName = L"";
	shaderName.assign(m_ShaderName.begin(), m_ShaderName.end());

	ID3DBlob* pVSBlob = nullptr;
	ShaderUtil::CreateVertexShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "VS", "vs_5_0", &pVSBlob, &m_pVertexShader);
	wrapDevice->GetDevice()->CreateInputLayout(LAYOUT_P0T0T1, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout);
	Safe_Release(pVSBlob);

	ID3DBlob* pHSBlob = nullptr;
	ShaderUtil::CreateHullShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "HS", "hs_5_0", &pHSBlob, &m_pHullShader);
	Safe_Release(pHSBlob);

	ID3DBlob* pDSBlob = nullptr;
	ShaderUtil::CreateDomainShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "DS", "ds_5_0", &pDSBlob, &m_pDomainShader);
	Safe_Release(pDSBlob);

	ID3DBlob* pPSBlob = nullptr;
	ShaderUtil::CreatePixelShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "PS", "ps_5_0", &pPSBlob, &m_pPixelShader);
	Safe_Release(pPSBlob);
}

void TerrainShaderRender::InitSampler()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	m_mapSampler.insert({ "SampleLinear", nullptr });
	wrapDevice->GetDevice()->CreateSamplerState(&sampDesc, &m_mapSampler["SampleLinear"]);

	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	m_mapSampler.insert({ "SampleHeightmap", nullptr });
	wrapDevice->GetDevice()->CreateSamplerState(&sampDesc, &m_mapSampler["SampleHeightmap"]);
}

void TerrainShaderRender::SetupConstant(const ConstantInfo* info)
{
	Camera& camera = Camera::GetInstance();	

	XMFLOAT4 worldPlanes[6];
	ExtractFrustumPlanes(worldPlanes, (camera.ViewProj()));

	TerrainTSBuffer tscb;
	tscb.ViewProj = XMMatrixTranspose(camera.ViewProj());
	tscb.EyePos = camera.GetPosition();
	tscb.DistTess = { 20.0f, 500.0f, 0.0f, 6.0f };
	tscb.TexScale = { 50.0f, 50.0f, 0.f, 0.f };

	for (int i = 0; i < 6; ++i)
	{
		tscb.WorldFrustumPlanes[i] = worldPlanes[i];
	}
	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["TerrainTSBuffer"], 0, NULL, &tscb, 0, 0);

	TerrainPSBuffer pscb;
	pscb.FogColor = { 0.75f, 0.75f, 0.75f, 1.0f };
	pscb.FogInfo = { 15.0f, 175.0f, 0.f, 0.f };
	pscb.gMaterial = info->material;

	
	pscb.TexelCellInfo = { 1.0f / info->heightmapWidth, 1.0f / info->heightmapHeight, info->cellSpacing, 0 };

	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["TerrainPSBuffer"], 0, NULL, &pscb, 0, 0);
}

void TerrainShaderRender::Render(ObjectResource* pResource, const RenderInfo* info)
{	
	ID3D11DeviceContext* context = wrapDevice->GetDeviceContext();

	context->IASetPrimitiveTopology(info->primitive_topology);
	context->IASetInputLayout(m_pVertexLayout);

	context->IASetVertexBuffers(0, 1, &pResource->pVertexBuffer, &info->stride, &info->offset);
	context->IASetIndexBuffer(pResource->pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	context->HSSetConstantBuffers(1, 1, &m_mapConstant["TerrainTSBuffer"]);
	context->DSSetConstantBuffers(1, 1, &m_mapConstant["TerrainTSBuffer"]);
	context->PSSetConstantBuffers(2, 1, &m_mapConstant["TerrainPSBuffer"]);

	context->VSSetShader(m_pVertexShader, NULL, 0);
	context->HSSetShader(m_pHullShader, NULL, 0);
	context->DSSetShader(m_pDomainShader, NULL, 0);
	context->PSSetShader(m_pPixelShader, NULL, 0);

	context->PSSetShaderResources(0, 1, &m_vecTextureSRV[0]);
	context->PSSetShaderResources(1, 1, &m_vecTextureSRV[1]);
	context->VSSetShaderResources(2, 1, &m_vecTextureSRV[2]);
	context->DSSetShaderResources(2, 1, &m_vecTextureSRV[2]);
	context->PSSetShaderResources(2, 1, &m_vecTextureSRV[2]);

	context->PSSetSamplers(0, 1, &m_mapSampler["SampleLinear"]);
	context->VSSetSamplers(1, 1, &m_mapSampler["SampleHeightmap"]);
	context->DSSetSamplers(1, 1, &m_mapSampler["SampleHeightmap"]);
	context->PSSetSamplers(1, 1, &m_mapSampler["SampleHeightmap"]);

	context->DrawIndexed(pResource->indexNum, 0, 0);

	context->HSSetShader(0, 0, 0);
	context->DSSetShader(0, 0, 0);
}

void TerrainShaderRender::ExtractFrustumPlanes(XMFLOAT4 planes[6], CXMMATRIX M)
{
	//
	// Left
	//
	planes[0].x = M(0, 3) + M(0, 0);
	planes[0].y = M(1, 3) + M(1, 0);
	planes[0].z = M(2, 3) + M(2, 0);
	planes[0].w = M(3, 3) + M(3, 0);

	//
	// Right
	//
	planes[1].x = M(0, 3) - M(0, 0);
	planes[1].y = M(1, 3) - M(1, 0);
	planes[1].z = M(2, 3) - M(2, 0);
	planes[1].w = M(3, 3) - M(3, 0);

	//
	// Bottom
	//
	planes[2].x = M(0, 3) + M(0, 1);
	planes[2].y = M(1, 3) + M(1, 1);
	planes[2].z = M(2, 3) + M(2, 1);
	planes[2].w = M(3, 3) + M(3, 1);

	//
	// Top
	//
	planes[3].x = M(0, 3) - M(0, 1);
	planes[3].y = M(1, 3) - M(1, 1);
	planes[3].z = M(2, 3) - M(2, 1);
	planes[3].w = M(3, 3) - M(3, 1);

	//
	// Near
	//
	planes[4].x = M(0, 2);
	planes[4].y = M(1, 2);
	planes[4].z = M(2, 2);
	planes[4].w = M(3, 2);

	//
	// Far
	//
	planes[5].x = M(0, 3) - M(0, 2);
	planes[5].y = M(1, 3) - M(1, 2);
	planes[5].z = M(2, 3) - M(2, 2);
	planes[5].w = M(3, 3) - M(3, 2);

	// Normalize the plane equations.
	for (int i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&planes[i]));
		XMStoreFloat4(&planes[i], v);
	}
}

void TerrainShaderRender::InitConstant()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(TerrainTSBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "TerrainTSBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["TerrainTSBuffer"]);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(TerrainPSBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "TerrainPSBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["TerrainPSBuffer"]);
}

//--------------------------

InstanceShaderRender::InstanceShaderRender()
{
	m_ShaderName = L"InstanceShader.fx";
	m_Renderkey = RENDER_KEY::INSTANCE;
}

void InstanceShaderRender::InitShader()
{
	D3D11_INPUT_ELEMENT_DESC LAYOUT_PNTWC[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	UINT numElements = ARRAYSIZE(LAYOUT_PNTWC);
	
	ID3DBlob* pVSBlob = nullptr;
	ShaderUtil::CreateVertexShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "VS", "vs_5_0", &pVSBlob, &m_pVertexShader);
	wrapDevice->GetDevice()->CreateInputLayout(LAYOUT_PNTWC, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout);
	Safe_Release(pVSBlob);

	ID3DBlob* pPSBlob = nullptr;
	ShaderUtil::CreatePixelShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "PS", "ps_5_0", &pPSBlob, &m_pPixelShader);
	Safe_Release(pPSBlob);
}

void InstanceShaderRender::SetupConstant(const ConstantInfo* info)
{
	Camera& camera = Camera::GetInstance();

	VPBuffer cb;
	cb.ViewProj = XMMatrixTranspose(camera.ViewProj());
	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["VPBuffer"], 0, NULL, &cb, 0, 0);
}

void InstanceShaderRender::Render(ObjectResource* pResource, const RenderInfo* info)
{
	ID3D11DeviceContext* context = wrapDevice->GetDeviceContext();
	//이건 수정해야 함 이쪽에서 하는일이 아님 인스턴스쪽에서 처리하자 임시!!
	UINT stride[2] = { sizeof(BoxVertex), sizeof(InstanceInfo) };
	UINT offset[2] = { 0, 0 };
	ID3D11Buffer* vbs[2] = { pResource->pVertexBuffer, pResource->pInstnaceBuffer };

	context->IASetInputLayout(m_pVertexLayout);
	context->IASetVertexBuffers(0, 2, vbs, stride, offset);
	context->IASetIndexBuffer(pResource->pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(info->primitive_topology);

	context->VSSetConstantBuffers(1, 1, &m_mapConstant["VPBuffer"]);
	context->VSSetShader(m_pVertexShader, NULL, 0);
	context->PSSetShader(m_pPixelShader, NULL, 0);

	context->DrawIndexedInstanced(pResource->indexNum, info->visibleObjectCount, 0, 0, 0);
}

void InstanceShaderRender::InitConstant()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VPBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "VPBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["VPBuffer"]);
}

//------------------------
BillboardShaderRender::BillboardShaderRender()
{
	m_ShaderName = L"TreeSprite.fx";
	m_Renderkey = RENDER_KEY::BILLBOARD;
}

BillboardShaderRender::~BillboardShaderRender()
{
	Safe_Release(m_AlphaToCoverageBS);
}

void BillboardShaderRender::InitShader()
{
	D3D11_INPUT_ELEMENT_DESC LAYOUT_P0S0[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(LAYOUT_P0S0);
	
	ID3DBlob* pVSBlob = nullptr;
	ShaderUtil::CreateVertexShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "VS", "vs_5_0", &pVSBlob, &m_pVertexShader);
	wrapDevice->GetDevice()->CreateInputLayout(LAYOUT_P0S0, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout);
	Safe_Release(pVSBlob);

	ID3DBlob* pGSBlob = nullptr;
	ShaderUtil::CreateGeometryShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "GS", "gs_5_0", &pGSBlob, &m_pGeometryShader);
	Safe_Release(pGSBlob);

	ID3DBlob* pPSBlob = nullptr;
	ShaderUtil::CreatePixelShader(wrapDevice->GetDevice(), m_ShaderName.c_str(), "PS", "ps_5_0", &pPSBlob, &m_pPixelShader);
	Safe_Release(pPSBlob);
}

void BillboardShaderRender::SetupConstant(const ConstantInfo* info)
{
	Camera& camera = Camera::GetInstance();	

	VPMPBuffer cb;
	cb.ViewProj = XMMatrixTranspose(camera.ViewProj());
	cb.Material = info->material;
	cb.EyePosW = camera.GetPosition();
	wrapDevice->GetDeviceContext()->UpdateSubresource(m_mapConstant["VPMPBuffer"], 0, NULL, &cb, 0, 0);
}

void BillboardShaderRender::Render(ObjectResource* pResource, const RenderInfo* info)
{
	ID3D11DeviceContext* context = wrapDevice->GetDeviceContext();

	context->IASetVertexBuffers(0, 1, &pResource->pVertexBuffer, &info->stride, &info->offset);
	context->IASetInputLayout(m_pVertexLayout);
	context->IASetPrimitiveTopology(info->primitive_topology);

	context->GSSetConstantBuffers(1, 1, &m_mapConstant["VPMPBuffer"]);
	context->PSSetConstantBuffers(1, 1, &m_mapConstant["VPMPBuffer"]);

	context->VSSetShader(m_pVertexShader, NULL, 0);
	context->GSSetShader(m_pGeometryShader, NULL, 0);
	context->PSSetShader(m_pPixelShader, NULL, 0);
	context->PSSetShaderResources(0, 1, &m_vecTextureSRV[0]);
	context->PSSetSamplers(0, 1, &m_mapSampler["SampleLinearClamp"]);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->OMSetBlendState(m_AlphaToCoverageBS, blendFactor, 0xffffffff);

	context->Draw(1, 0);	//일단은 트리 1개만 띄워보자
	context->GSSetShader(0, NULL, 0);
	context->OMSetBlendState(0, blendFactor, 0xffffffff);
}

void BillboardShaderRender::InitSampler()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = -FLT_MAX;
	sampDesc.MaxLOD = FLT_MAX;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.BorderColor[0] = 1.0f;
	sampDesc.BorderColor[1] = 1.0f;
	sampDesc.BorderColor[2] = 1.0f;
	sampDesc.BorderColor[3] = 1.0f;

	m_mapSampler.insert({ "SampleLinearClamp", nullptr });
	wrapDevice->GetDevice()->CreateSamplerState(&sampDesc, &m_mapSampler["SampleLinearClamp"]);
}

void BillboardShaderRender::InitEtc()
{
	D3D11_BLEND_DESC alphaToCoverageDesc = { 0 };
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	wrapDevice->GetDevice()->CreateBlendState(&alphaToCoverageDesc, &m_AlphaToCoverageBS);
}

void BillboardShaderRender::InitConstant()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VPMPBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	m_mapConstant.insert({ "VPMPBuffer", nullptr });
	wrapDevice->GetDevice()->CreateBuffer(&bd, NULL, &m_mapConstant["VPMPBuffer"]);
}
