#include "stdafx.h"
#include "Lighting.h"
#include "DeviceDx11.h"
#include "Camera.h"

Lighting::Lighting()
	:m_lightConstantBuffer(nullptr), m_constantInfoSize(0), wrapDevice(nullptr)
{}


Lighting::~Lighting()
{
	CleanupApp();
}

void Lighting::CleanupApp()
{
	Safe_Release(m_lightConstantBuffer);
}

void Lighting::Init()
{
	m_lightConstantInfo.mDirLight[0].Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_lightConstantInfo.mDirLight[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_lightConstantInfo.mDirLight[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	m_lightConstantInfo.mDirLight[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	m_lightConstantInfo.mDirLight[1].Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_lightConstantInfo.mDirLight[1].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_lightConstantInfo.mDirLight[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_lightConstantInfo.mDirLight[1].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	m_lightConstantInfo.mDirLight[2].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_lightConstantInfo.mDirLight[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_lightConstantInfo.mDirLight[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_lightConstantInfo.mDirLight[2].Direction = XMFLOAT3(-0.57735f, -0.57735f, -0.57735f);

	m_constantInfoSize = sizeof(DirLightBuffer);
	// Create the constant buffer	

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = m_constantInfoSize;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HRESULT hr = DeviceDx11::GetInstance().GetDevice()->CreateBuffer(&bd, NULL, &m_lightConstantBuffer);
}

void Lighting::Adjust()
{	 
	wrapDevice->GetDeviceContext()->UpdateSubresource(m_lightConstantBuffer, 0, NULL, &m_lightConstantInfo, 0, 0);
	wrapDevice->GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_lightConstantBuffer);	//모든 광원 관련 객체의 상수버퍼 0번을 광원으로 설정
}

void Lighting::Update()
{
	m_lightConstantInfo.mEyePosW = Camera::GetInstance().GetPosition();
}

