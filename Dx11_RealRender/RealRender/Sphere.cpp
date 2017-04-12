#include "stdafx.h"
#include "Sphere.h"
#include "ShareValue.h"
#include "DeviceDx11.h"
#include "TemplateUtil.h"
#include "ShaderUtil.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "RenderManager.h"

////////////////////////////////////////////////////////
// Sphere
////////////////////////////////////////////////////////
#pragma region Sphere
Sphere::Sphere() 
	: m_Radius(10), m_SliceCount(10), m_StackCount(10)
{}

Sphere::~Sphere()
{
	Cleanup();
}

void Sphere::Init()
{
	m_ObjectName = "Sphere";
	m_RenderKey = RENDER_KEY::SOLID;
	Object::Init();
}

void Sphere::Update(float dt)
{
	m_World = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
}

void Sphere::Render()
{
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void Sphere::Cleanup(){}

void Sphere::InitResource(ObjectResource* resource)
{
	InitBuffer(resource);
}

void Sphere::InitBuffer(ObjectResource* resource)
{		
	SphereVertex topVertex;
	topVertex.Pos = XMFLOAT3(0.0f, +m_Radius, 0.0f);
	topVertex.Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	topVertex.Tex = XMFLOAT2(1.0f, 0.0f);
	topVertex.Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	SphereVertex bottomVertex;
	bottomVertex.Pos = XMFLOAT3(0.0f, -m_Radius, 0.0f);
	bottomVertex.Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
	bottomVertex.Tex = XMFLOAT2(1.0f, 0.0f);
	bottomVertex.Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);

	vector<SphereVertex> vertices;
	vertices.push_back(topVertex);
	float phiStep = XM_PI / m_StackCount;
	float thetaStep = 2.0f*XM_PI / m_SliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (int i = 1; i <= m_StackCount - 1; ++i)
	{
		float phi = i*phiStep;

		// Vertices of ring.
		for (int j = 0; j <= m_SliceCount; ++j)
		{
			float theta = j*thetaStep;

			SphereVertex v;

			// spherical to cartesian
			v.Pos.x = m_Radius*sinf(phi)*cosf(theta);
			v.Pos.y = m_Radius*cosf(phi);
			v.Pos.z = m_Radius*sinf(phi)*sinf(theta);
			v.Normal = XMFLOAT3(0, 0, 0);

			v.Tangent.x = -m_Radius*sinf(phi)*sinf(theta);
			v.Tangent.y = 0.0f;
			v.Tangent.z = +m_Radius*sinf(phi)*cosf(theta);
			// Partial derivative of P with respect to theta
			XMVECTOR T = XMLoadFloat3(&v.Tangent);
			XMStoreFloat3(&v.Tangent, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat3(&v.Pos);
			XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

			v.Tex.x = theta / XM_2PI;
			v.Tex.y = phi / XM_PI;

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottomVertex);

	//--------------------------------------------------------------------
	vector<WORD> indices;
	for (int i = 1; i <= m_SliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	int baseIndex = 1;
	int ringVertexCount = m_SliceCount + 1;
	for (int i = 0; i < m_StackCount - 2; ++i)
	{
		for (int j = 0; j < m_SliceCount; ++j)
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

	for (int i = 0; i < m_SliceCount; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}
	resource->indexNum = indices.size();

	//Stack Slice 최소값 검증 
	assert(m_StackCount >= 3);
	assert(m_SliceCount >= 2);

	const int numTriangles = (m_StackCount * m_SliceCount) + ((m_StackCount - 2) * m_SliceCount);
	for (int i = 0; i < numTriangles; ++i)
	{
		int i0 = indices[i * 3 + 0];
		int i1 = indices[i * 3 + 1];
		int i2 = indices[i * 3 + 2];

		XMVECTOR v0 = XMLoadFloat3(&vertices[i0].Pos);
		XMVECTOR v1 = XMLoadFloat3(&vertices[i1].Pos);
		XMVECTOR v2 = XMLoadFloat3(&vertices[i2].Pos);

		XMVECTOR e0 = XMVectorSubtract(v1, v0);
		XMVECTOR e1 = XMVectorSubtract(v2, v0);
		XMVECTOR  faceNormal = XMVector3Cross(e0, e1);

		XMVECTOR normal = XMLoadFloat3(&vertices[i0].Normal);
		XMStoreFloat3(&vertices[i0].Normal, XMVectorAdd(normal, faceNormal));

		normal = XMLoadFloat3(&vertices[i1].Normal);
		XMStoreFloat3(&vertices[i1].Normal, XMVectorAdd(normal, faceNormal));

		normal = XMLoadFloat3(&vertices[i2].Normal);
		XMStoreFloat3(&vertices[i2].Normal, XMVectorAdd(normal, faceNormal));
	}

	const int numVertices = vertices.size();
	for (int i = 0; i < numVertices; ++i)
	{
		XMStoreFloat3(&vertices[i].Normal, XMVector3Normalize(XMLoadFloat3(&vertices[i].Normal)));
	}
	/////////////////////

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(SphereVertex) * vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	wrapDevice->GetDevice()->CreateBuffer(&bd, &vinitData, &resource->pVertexBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(WORD) * indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	wrapDevice->GetDevice()->CreateBuffer(&bd, &iinitData, &resource->pIndexBuffer);
}

void Sphere::SetupConstant()
{
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

const RenderInfo* Sphere::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(SphereVertex);
	m_RenderInfo.offset = 0;

	return &m_RenderInfo;
}

const ConstantInfo* Sphere::GetConstantInfo()
{
	m_ConstantInfo.world = m_World;
	m_ConstantInfo.color = { 1, 0.5f, 0.3f, 1 };
	return &m_ConstantInfo;
}

#pragma endregion Sphere

////////////////////////////////////////////////////////
// LightSphere
////////////////////////////////////////////////////////
#pragma region LightSphere
LightSphere::LightSphere(){}

LightSphere::~LightSphere()
{
	Cleanup();
}

void LightSphere::Init()
{
	m_ObjectName = "LightSphere";
	m_RenderKey = RENDER_KEY::LIGHT;
	Object::Init();
}

void LightSphere::Update(float dt)
{
	Sphere::Update(dt);
}

void LightSphere::Render()
{
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void LightSphere::Cleanup()
{
}

void LightSphere::InitResource(ObjectResource* resource)
{
	Sphere::InitBuffer(resource);	
}

void LightSphere::SetupConstant()
{
	Camera& camera = Camera::GetInstance();

	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

const RenderInfo* LightSphere::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(SphereVertex);
	m_RenderInfo.offset = 0;
	m_RenderInfo.primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return &m_RenderInfo;
}

const ConstantInfo* LightSphere::GetConstantInfo()
{
	m_ConstantInfo.world = m_World;
	m_ConstantInfo.color = { 0, 0, 0, 0 };
	m_ConstantInfo.material.Ambient = XMFLOAT4(0.2f, 0.57f, 0.86f, 1.0f);
	m_ConstantInfo.material.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_ConstantInfo.material.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_ConstantInfo.material.Reflect = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	return &m_ConstantInfo;
}

#pragma endregion LightSphere

////////////////////////////////////////////////////////
// ReflectSphere
////////////////////////////////////////////////////////
#pragma region ReflectSphere
void ReflectSphere::Init()
{
	m_ObjectName = "ReflectSphere";
	m_RenderKey = RENDER_KEY::REFLECT;
	Object::Init();
}

void ReflectSphere::Update(float dt)
{
	LightSphere::Update(dt);
}

void ReflectSphere::Render()
{	 
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void ReflectSphere::Cleanup(){}

void ReflectSphere::InitResource(ObjectResource* resource)
{
	Sphere::InitBuffer(resource);	
	InitTexture(resource);
}

void ReflectSphere::SetupConstant()
{	
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

void ReflectSphere::InitTexture(ObjectResource* resource)
{	 
	ID3D11ShaderResourceView* pTextureSRV = nullptr;		
	D3DX11CreateShaderResourceViewFromFile(wrapDevice->GetDevice(), L"Textures\\grasscube1024.dds", NULL, NULL, &pTextureSRV, NULL);
	RenderManager::GetInstance().AddTextureSRV(m_RenderKey, pTextureSRV);
}

const RenderInfo* ReflectSphere::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(SphereVertex);
	m_RenderInfo.offset = 0;
	m_RenderInfo.primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return &m_RenderInfo;
}

const ConstantInfo* ReflectSphere::GetConstantInfo()
{
	m_ConstantInfo.world = m_World;
	m_ConstantInfo.color = { 0, 0, 0, 0 };
	m_ConstantInfo.material.Ambient = XMFLOAT4(0.2f, 0.57f, 0.86f, 1.0f);
	m_ConstantInfo.material.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_ConstantInfo.material.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_ConstantInfo.material.Reflect = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	return &m_ConstantInfo;
}

#pragma endregion ReflectSphere

////////////////////////////////////////////////////////
// DisplacementSphere
////////////////////////////////////////////////////////
#pragma region DisplacementSphere
void DisplacementSphere::Init()
{
	m_ObjectName = "DisplacementSphere";
	m_RenderKey = RENDER_KEY::DISPLACEMENT;
	Object::Init();
}

void DisplacementSphere::Update(float dt)
{
	Sphere::Update(dt);
}

void DisplacementSphere::Render()
{
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void DisplacementSphere::Cleanup(){}

void DisplacementSphere::InitResource(ObjectResource* resource)
{
	Sphere::InitBuffer(resource);
	InitTexture(resource);	
}

void DisplacementSphere::InitTexture(ObjectResource* resource)
{		
	ID3D11ShaderResourceView* pTextureSRV = nullptr;
	D3DX11CreateShaderResourceViewFromFile(wrapDevice->GetDevice(), L"Textures\\bricks.dds", NULL, NULL, &pTextureSRV, NULL);
	RenderManager::GetInstance().AddTextureSRV(m_RenderKey, pTextureSRV);
		
	D3DX11CreateShaderResourceViewFromFile(wrapDevice->GetDevice(), L"Textures\\bricks_nmap.dds", NULL, NULL, &pTextureSRV, NULL);
	RenderManager::GetInstance().AddTextureSRV(m_RenderKey, pTextureSRV);
}

void DisplacementSphere::SetupConstant()
{
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

const RenderInfo* DisplacementSphere::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(SphereVertex);
	m_RenderInfo.offset = 0;
	m_RenderInfo.primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;

	return &m_RenderInfo;
}

const ConstantInfo* DisplacementSphere::GetConstantInfo()
{
	m_ConstantInfo.world = m_World;
	m_ConstantInfo.color = { 0, 0, 0, 0 };
	m_ConstantInfo.material.Ambient	= XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_ConstantInfo.material.Diffuse	= XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_ConstantInfo.material.Specular= XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	m_ConstantInfo.material.Reflect	= XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

	return &m_ConstantInfo;
}

#pragma endregion DisplacementSphere