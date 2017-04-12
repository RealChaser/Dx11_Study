#include "stdafx.h"
#include "ShadowDepth.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "DeviceDx11.h"

ShadowDepth::ShadowDepth()
{
}


ShadowDepth::~ShadowDepth()
{}

void ShadowDepth::Init()
{
	m_ObjectName = "ShadowDepth";
	m_RenderKey = RENDER_KEY::SHADOW;
	
	Object::Init();
}

void ShadowDepth::Update(float dt){}

void ShadowDepth::Render()
{
	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void ShadowDepth::Cleanup(){}

void ShadowDepth::InitResource(ObjectResource* resource)
{
	InitBuffer(resource);
}

void ShadowDepth::InitBuffer(ObjectResource* resource)
{
	ShadowMapDepthVertex vertices[] = { { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
										{ XMFLOAT3(-1.0f, +1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
										{ XMFLOAT3(+1.0f, +1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
										{ XMFLOAT3(+1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }	};

	WORD indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	resource->indexNum = sizeof(indices) / sizeof(WORD);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ShadowMapDepthVertex)* 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	wrapDevice->GetDevice()->CreateBuffer(&bd, &InitData, &resource->pVertexBuffer);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD)* resource->indexNum; 
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	wrapDevice->GetDevice()->CreateBuffer(&bd, &InitData, &resource->pIndexBuffer);
}

const RenderInfo* ShadowDepth::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(ShadowMapDepthVertex);
	m_RenderInfo.offset = 0;

	return &m_RenderInfo;
}

const ConstantInfo* ShadowDepth::GetConstantInfo()
{
	m_World = XMMATRIX(
		0.3f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.3f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.7f, -0.7f, 0.0f, 1.0f);

	m_ConstantInfo.world = m_World;
	
	return &m_ConstantInfo;
}

void ShadowDepth::SetupConstant()
{
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}


