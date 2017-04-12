#include "stdafx.h"
#include "Sky.h"
#include "ShareValue.h"
#include "DeviceDx11.h"
#include "TemplateUtil.h"
#include "ShaderUtil.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "RenderManager.h"

Sky::Sky(){}

Sky::~Sky()
{
	Cleanup();
}

void Sky::Init()
{
	m_ObjectName = "Sky";
	m_RenderKey = RENDER_KEY::SKY;
	Object::Init();
}

void Sky::Update(float dt){}

void Sky::Render()
{	
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void Sky::Cleanup()
{
	Safe_Release(m_SkyRS);
	Safe_Release(m_SkyDSS);
}

void Sky::InitResource(ObjectResource* resource)
{
	InitBuffer(resource);
	InitTexture(resource);	
}

void Sky::InitBuffer(ObjectResource* resource)
{
	//임시 상수
	const float radius = 500.f;	// 구의 반경
	const int sliceCount = 30;	// 
	const int stackCount = 30;

	/////////////////
	//정점 버퍼 만들기
	/////////////////
	XMFLOAT3 topVertex(0.0f, +radius, 0.0f);
	XMFLOAT3 bottomVertex(0.0f, -radius, 0.0f);

	vector<XMFLOAT3> vertices;
	vertices.push_back(topVertex);
	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f*XM_PI / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i*phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j*thetaStep;

			XMFLOAT3 v;

			// spherical to cartesian
			v.x = radius*sinf(phi)*cosf(theta);
			v.y = radius*cosf(phi);
			v.z = radius*sinf(phi)*sinf(theta);

			// Partial derivative of P with respect to theta
			
			vertices.push_back(v);
		}
	}

	vertices.push_back(bottomVertex);
	

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	wrapDevice->GetDevice()->CreateBuffer(&vbd, &vinitData, &resource->pVertexBuffer);

	/////////////////
	//인덱스 버퍼 만들기
	/////////////////
	vector<WORD> indices;
	for (int i = 1; i <= sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	
	int baseIndex = 1;
	int ringVertexCount = sliceCount + 1;
	for (int i = 0; i < stackCount - 2; ++i)
	{
		for (int j = 0; j < sliceCount; ++j)
		{
			indices.push_back(baseIndex + i*ringVertexCount + j);
			indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

			indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
			indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
		}
	}
	
	int southPoleIndex = (int) vertices.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;

	for (int i = 0; i < sliceCount; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}
	
	resource->indexNum = indices.size();
	
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(WORD) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags = 0;	

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];

	wrapDevice->GetDevice()->CreateBuffer(&ibd, &iinitData, &resource->pIndexBuffer);		
}

void Sky::InitTexture(ObjectResource* resource)
{	 
	ID3D11ShaderResourceView* pTextureSRV = nullptr;
	D3DX11CreateShaderResourceViewFromFile(wrapDevice->GetDevice(), L"Textures\\grasscube1024.dds", NULL, NULL, &pTextureSRV, NULL);
	RenderManager::GetInstance().AddTextureSRV(m_RenderKey, pTextureSRV);
}

void Sky::SetupConstant()
{	
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

const RenderInfo* Sky::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(XMFLOAT3);
	m_RenderInfo.offset = 0;
	m_RenderInfo.primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return &m_RenderInfo;
}

const ConstantInfo* Sky::GetConstantInfo()
{
	m_ConstantInfo.world = m_World;	

	return &m_ConstantInfo;
}

