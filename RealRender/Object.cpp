#include "stdafx.h"
#include "Object.h"
#include "ResourceManager.h"
#include "DeviceDx11.h"

DeviceDx11* Object::wrapDevice = nullptr;

Object::Object() 
	:	m_Position(0.f, 0.f, 0.f)
{}

Object::~Object()
{
	Cleanup();
}

void Object::Init()
{	
	assert(m_ObjectName.length() != 0);
	CONSOLE_PRINT((m_ObjectName + " »ý¼º\n").c_str())

	ResourceManager& resourceManager = ResourceManager::GetInstance();
	if (!resourceManager.CheckExist(m_ObjectName))
	{
		ObjectResource* resource = new ObjectResource();
		InitResource(resource);

		resourceManager.Add({ m_ObjectName, resource });
	}	

	m_World = XMMatrixIdentity();
}

void Object::Cleanup()
{		
	wrapDevice = nullptr;
}

void Object::Render()
{
	if (GetAsyncKeyState('1') & 0x8000)
	{
		D3D11_RASTERIZER_DESC wireframeDesc;
		ID3D11RasterizerState* wireframeRS;
		ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
		wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
		wireframeDesc.CullMode = D3D11_CULL_BACK;
		wireframeDesc.FrontCounterClockwise = false;
		wireframeDesc.DepthClipEnable = true;
		wrapDevice->GetDevice()->CreateRasterizerState(&wireframeDesc, &wireframeRS);
		wrapDevice->GetDeviceContext()->RSSetState(wireframeRS);
	}
}

