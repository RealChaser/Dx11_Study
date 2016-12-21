#include "stdafx.h"
#include "Box.h"
#include "ShareValue.h"
#include "DeviceDx11.h"
#include "TemplateUtil.h"
#include "ShaderUtil.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "Util.h"

////////////////////////////////////////////////////////
// Box
////////////////////////////////////////////////////////
#pragma region Box

Box::Box(){}
Box::~Box()
{ 
	Cleanup();	
}

void Box::Init()
{
	m_ObjectName = "Box";
	m_RenderKey = RENDER_KEY::SOLID;
	Object::Init();	
}

void Box::Update(float dt)
{
	static float t = 0.0f;
	t += (dt * 0.5f);

	XMMATRIX matScaling = XMMatrixIdentity();
	matScaling._11 = matScaling._22 = matScaling._33 = 3.f;

	m_World = matScaling * XMMatrixRotationY(t) * XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
}

void Box::Render()
{
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);

	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void Box::Cleanup(){}

void Box::InitBuffer(ObjectResource* resource)
{
	WORD indices[] =
	{
		3, 1, 0, 2, 1, 3,
		6, 4, 5, 7, 4, 6,
		11, 9, 8, 10, 9, 11,
		14, 12, 13, 15, 12, 14,
		19, 17, 16, 18, 17, 19,
		22, 20, 21, 23, 20, 22
	};

	resource->indexNum = sizeof(indices) / sizeof(WORD);

	BoxVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
	};

	//평균 노멀 계산--------------------------
	const int numTriangles = 12; // 박스라서 무조건 12개	
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

	const int numVertices = 8; // 박스라서 정점 8개	
	for (int i = 0; i < numVertices; ++i)
	{
		XMStoreFloat3(&vertices[i].Normal, XMVector3Normalize(XMLoadFloat3(&vertices[i].Normal)));
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(BoxVertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	wrapDevice->GetDevice()->CreateBuffer(&bd, &InitData, &resource->pVertexBuffer);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * resource->indexNum;    // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	wrapDevice->GetDevice()->CreateBuffer(&bd, &InitData, &resource->pIndexBuffer);
}

void Box::InitResource(ObjectResource* resource)
{
	InitBuffer(resource);		
}

void Box::SetupConstant()
{		
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

const RenderInfo* Box::GetRenderInfo()
{	
	m_RenderInfo.stride = sizeof(BoxVertex);
	m_RenderInfo.offset = 0;
	
	return &m_RenderInfo;
}

const ConstantInfo* Box::GetConstantInfo()
{		
	m_ConstantInfo.world = m_World;	
	m_ConstantInfo.color = { 1, 0, 0, 1 };
	return &m_ConstantInfo;
}

#pragma endregion Box

////////////////////////////////////////////////////////
// LightBox
////////////////////////////////////////////////////////
#pragma region LightBox

LightBox::LightBox(){}

LightBox::~LightBox()
{
	Cleanup();
}

void LightBox::Init()
{
	m_ObjectName = "LightBox";
	m_RenderKey = RENDER_KEY::LIGHT;
	Object::Init();
}

void LightBox::Update(float dt)
{
	Box::Update(dt);
}

void LightBox::Render()
{	 
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void LightBox::Cleanup(){}

void LightBox::InitResource(ObjectResource* resource)
{
	Box::InitBuffer(resource);
}

void LightBox::SetupConstant()
{
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}
const RenderInfo* LightBox::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(BoxVertex);
	m_RenderInfo.offset = 0;

	return &m_RenderInfo;
}

const ConstantInfo* LightBox::GetConstantInfo()
{
	m_ConstantInfo.world = m_World;
	m_ConstantInfo.color = { 0, 0, 0, 0 };
	m_ConstantInfo.material.Ambient = XMFLOAT4(0.2f, 0.27f, 0.16f, 1.0f);
	m_ConstantInfo.material.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_ConstantInfo.material.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 16.0f);
	m_ConstantInfo.material.Reflect = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	return &m_ConstantInfo;
}

#pragma endregion LightBox

////////////////////////////////////////////////////////
// TextureBox
////////////////////////////////////////////////////////
#pragma region TextureBox

TextureBox::TextureBox(){}

TextureBox::~TextureBox()
{
	Cleanup();
}

void TextureBox::Init()
{
	m_ObjectName = "TextureBox";
	m_RenderKey = RENDER_KEY::TEXTURE;
	Object::Init();
}

void TextureBox::Update(float dt)
{
	Box::Update(dt);
}

void TextureBox::Render()
{	 
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void TextureBox::Cleanup(){}

void TextureBox::InitResource(ObjectResource* resource)
{
	Box::InitBuffer(resource);	
	InitTexture(resource);
}

void TextureBox::SetupConstant()
{	
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

void TextureBox::InitTexture(ObjectResource* resource)
{	 
	ID3D11ShaderResourceView* pTextureSRV = nullptr;	
	D3DX11CreateShaderResourceViewFromFile(wrapDevice->GetDevice(), L"Textures\\seafloor.dds", NULL, NULL, &pTextureSRV, NULL);
	RenderManager::GetInstance().AddTextureSRV(m_RenderKey, pTextureSRV);
}

const RenderInfo* TextureBox::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(BoxVertex);
	m_RenderInfo.offset = 0;
	m_RenderInfo.primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return &m_RenderInfo;
}

const ConstantInfo* TextureBox::GetConstantInfo()
{
	m_ConstantInfo.world = m_World;

	return &m_ConstantInfo;
}

#pragma endregion TextureBox

////////////////////////////////////////////////////////
// InstanceBox
////////////////////////////////////////////////////////
#pragma region InstanceBox
InstanceBox::InstanceBox(){}

InstanceBox::~InstanceBox()
{
	Cleanup();
}

void InstanceBox::Init()
{
	m_ObjectName = "InstanceBox";
	m_RenderKey = RENDER_KEY::INSTANCE;
	Object::Init();
}

void InstanceBox::Update(float dt)
{
	Box::Update(dt);
	 
	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	wrapDevice->GetDeviceContext()->Map(pResource->pInstnaceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	InstanceInfo* dataView = reinterpret_cast<InstanceInfo*>(mappedData.pData);

	m_visibleObjectCount = 0;
	for (UINT i = 0; i < m_instancedData.size(); ++i)
	{
		dataView[m_visibleObjectCount++] = m_instancedData[i];
	}

	wrapDevice->GetDeviceContext()->Unmap(pResource->pInstnaceBuffer, 0);
}

void InstanceBox::Render()
{	 
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void InstanceBox::Cleanup(){}

void InstanceBox::InitResource(ObjectResource* resource)
{
	Box::InitBuffer(resource);
	InitInstance(resource);
}

void InstanceBox::InitInstance(ObjectResource* resource)
{
	const int n = 5;
	m_instancedData.resize(n*n*n);

	float width = 20.0f;
	float height = 20.0f;
	float depth = 20.0f;

	float x = -0.5f*width + m_Position.x;
	float y = -0.5f*height + m_Position.y;
	float z = -0.5f*depth + m_Position.z;
	float dx = width / (n - 1);
	float dy = height / (n - 1);
	float dz = depth / (n - 1);
	for (int k = 0; k < n; ++k)
	{
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				// Position instanced along a 3D grid.
				m_instancedData[k*n*n + i*n + j].World = XMFLOAT4X4(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					x + j*dx, y + i*dy, z + k*dz, 1.0f);

				// Random color.
				m_instancedData[k*n*n + i*n + j].Color.x = Util::RandF(0.0f, 1.0f);
				m_instancedData[k*n*n + i*n + j].Color.y = Util::RandF(0.0f, 1.0f);
				m_instancedData[k*n*n + i*n + j].Color.z = Util::RandF(0.0f, 1.0f);
				m_instancedData[k*n*n + i*n + j].Color.w = 1.0f;
			}
		}
	}

	D3D11_BUFFER_DESC inbd;
	inbd.Usage = D3D11_USAGE_DYNAMIC;
	inbd.ByteWidth = sizeof(InstanceInfo) * m_instancedData.size();
	inbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	inbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	inbd.MiscFlags = 0;
	inbd.StructureByteStride = 0;

	wrapDevice->GetDevice()->CreateBuffer(&inbd, NULL, &resource->pInstnaceBuffer);
}

void InstanceBox::SetupConstant()
{
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

const RenderInfo* InstanceBox::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(SimpleVertex);
	m_RenderInfo.offset = 0;
	m_RenderInfo.visibleObjectCount = m_visibleObjectCount;
	m_RenderInfo.primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return &m_RenderInfo;
}

const ConstantInfo* InstanceBox::GetConstantInfo()
{
	//

	return &m_ConstantInfo;
}

#pragma endregion InstanceBox

////////////////////////////////////////////////////////
// AABB
////////////////////////////////////////////////////////
#pragma region AABB
AABB::AABB(){}

AABB::~AABB()
{
	Cleanup();
}

void AABB::Init()
{
	m_ObjectName = "AABB";
	m_RenderKey = RENDER_KEY::SOLID;
	Object::Init();

	m_Color = { 0, 0, 0, 1 };
}

void AABB::Update(float dt){}

void AABB::Render()
{	
	wrapDevice->GetDeviceContext()->RSSetState(m_WireframeRS);

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
	
	wrapDevice->GetDeviceContext()->RSSetState(nullptr);
}

void AABB::Cleanup()
{	
	Safe_Release(m_WireframeRS);
}

void AABB::InitResource(ObjectResource* resource)
{
	InitBuffer(resource);		

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_NONE;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;
	 
	wrapDevice->GetDevice()->CreateRasterizerState(&wireframeDesc, &m_WireframeRS);
}

void AABB::InitBuffer(ObjectResource* resource)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(SimpleVertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	wrapDevice->GetDevice()->CreateBuffer(&bd, nullptr, &resource->pVertexBuffer);

	//   /7---6
	//  4---5/|
	//  |/3-|-2
	//  0---1/
	// Create index buffer
	WORD indices[] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,

		4, 5,
		5, 6,
		6, 7,
		7, 4,

		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	resource->indexNum = sizeof(indices) / sizeof(WORD);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * resource->indexNum;  // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = indices;
	wrapDevice->GetDevice()->CreateBuffer(&bd, &InitData, &resource->pIndexBuffer);	
}

void AABB::SetMinMax(XMFLOAT3 min, XMFLOAT3 max)
{
	if (min.x > max.x || min.y > max.y || min.z > max.z)
	{
		CONSOLE_PRINT("min is bigger than max")
			return;
	}

	m_Vertices[0].Pos = min;							//렌더링을 위한 AABB좌표
	m_Vertices[1].Pos = XMFLOAT3(max.x, min.y, min.z);
	m_Vertices[2].Pos = XMFLOAT3(max.x, min.y, max.z);
	m_Vertices[3].Pos = XMFLOAT3(min.x, min.y, max.z);

	m_Vertices[4].Pos = XMFLOAT3(min.x, max.y, min.z);
	m_Vertices[5].Pos = XMFLOAT3(max.x, max.y, min.z);
	m_Vertices[6].Pos = max;
	m_Vertices[7].Pos = XMFLOAT3(min.x, max.y, max.z);

	XMFLOAT3 center((min.x + max.x) / 2.f, (min.y + max.y) / 2.f, (min.z + max.z) / 2.f);
	XMFLOAT3 scaling(center.x - min.x, center.y - min.y, center.z - min.z);

	//AABB는 회전이 없기 때문에 스케일과 이동만 고려한다.
	m_BoxWorld = XMMatrixScaling(scaling.x, scaling.y, scaling.z) * XMMatrixTranslation(center.x, center.y, center.z);
		 
	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	wrapDevice->GetDeviceContext()->Map(pResource->pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	SimpleVertex* dataView = reinterpret_cast<SimpleVertex*>(mappedData.pData);

	int i = 0;
	for each (auto vertex in m_Vertices)
	{
		dataView[i++] = vertex;
	}

	wrapDevice->GetDeviceContext()->Unmap(pResource->pVertexBuffer, 0);
}

XMFLOAT3 AABB::GetMin(vector<XMFLOAT3> vertices)
{
	float minX = vertices[0].x;
	float minY = vertices[0].y;
	float minZ = vertices[0].z;

	for (XMFLOAT3 vertex : vertices)
	{
		if (vertex.x < minX)
		{
			minX = vertex.x;
		}

		if (vertex.y < minY)
		{
			minY = vertex.y;
		}

		if (vertex.z < minZ)
		{
			minZ = vertex.z;
		}
	}

	return XMFLOAT3(minX, minY, minZ);
}

XMFLOAT3 AABB::GetMax(vector<XMFLOAT3> vertices)
{
	float maxX = vertices[0].x;
	float maxY = vertices[0].y;
	float maxZ = vertices[0].z;

	for (XMFLOAT3 vertex : vertices)
	{
		if (vertex.x > maxX)
		{
			maxX = vertex.x;
		}

		if (vertex.y > maxY)
		{
			maxY = vertex.y;
		}

		if (vertex.z > maxZ)
		{
			maxZ = vertex.z;
		}
	}

	return XMFLOAT3(maxX, maxY, maxZ);
}

void AABB::CalculateAABB(vector<XMFLOAT3> vertices)
{
	SetMinMax(GetMin(vertices), GetMax(vertices));
}

bool AABB::IntersectRayAxisAlignedBox(XMVECTOR rayPos, XMVECTOR rayDir)
{
	//광선의 공간을 AABB의 로컬공간으로 변환
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(m_BoxWorld), m_BoxWorld);
	rayPos = XMVector3TransformCoord(rayPos, invWorld);
	rayDir = XMVector3TransformNormal(rayDir, invWorld);

	//XMVECTOR -> XMFLOAT3 -> Arr-----------------------------------------------
	XMFLOAT3 rayNormal;
	XMFLOAT3 rayPoint;
	rayDir = XMVector3Normalize(rayDir);

	XMStoreFloat3(&rayNormal, rayDir);
	XMStoreFloat3(&rayPoint, rayPos);

	float arrRayNormal[3] = { rayNormal.x, rayNormal.y, rayNormal.z };
	float arrRayPoint[3] = { rayPoint.x, rayPoint.y, rayPoint.z };
	//--------------------------------------------------------------------------

	float t_min = 0;
	float t_max = FLT_MAX;

	for (int i = 0; i < 3; ++i)
	{
		//광선의 기울기가 거의 0인지 결정 하고
		//기울기가 거의 없다면 광선의 시작점이 박스 안에서 시작하는지 여부 판단(박스 안에서 광선의 시작 이라면 교차한다.)		
		if (abs(arrRayNormal[i]) < EPSILON)
		{	//로컬 좌표이기 때문에 -1보다 작거나 1보다 크면 박스 외부
			if (arrRayPoint[i] < -1.f || arrRayPoint[i] > 1.f)
			{
				return false;
			}
		}
		else
		{
			float denom = 1.0f / arrRayNormal[i];
			float t1 = (-arrRayPoint[i] - (-1.f)) * denom;
			float t2 = (-arrRayPoint[i] - (1.f)) * denom;

			if (t1 > t2)
			{
				swap(t1, t2);
			}

			t_min = max(t_min, t1);
			t_max = min(t_max, t2);

			if (t_min >= t_max)
				return false;
		}
	}

	//p = o + t_min * d;	//광선이 처음으로 부딪친 평면상의 점 위치
	return true;
}

void AABB::SetColor(XMFLOAT4 color)
{
	m_Color = color;
}

const RenderInfo* AABB::GetRenderInfo()
{	
	m_RenderInfo.stride = sizeof(SimpleVertex);
	m_RenderInfo.offset = 0;
	m_RenderInfo.primitive_topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	return &m_RenderInfo;
}

void AABB::SetupConstant()
{
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

const ConstantInfo* AABB::GetConstantInfo()
{
	m_ConstantInfo.world = m_World;
	m_ConstantInfo.color = { 0, 1, 0, 1 };
	return &m_ConstantInfo;
}

#pragma endregion AABB

////////////////////////////////////////////////////////
// PickingBox
////////////////////////////////////////////////////////
#pragma region PickingBox
PickingBox::PickingBox(){}

PickingBox::~PickingBox()
{
	Cleanup();
}

void PickingBox::Init()
{
	m_ObjectName = "PickingBox";
	m_RenderKey = RENDER_KEY::SOLID;
	Object::Init();
}

void PickingBox::Update(float dt)
{
	Box::Update(dt);

	XMFLOAT3 float3;
	vector<XMFLOAT3> vec;
	for (UINT i = 0; i < m_Vertices.size(); ++i)
	{	//XMVECTOR관련 래퍼 클래스 만들어야겠다....
		XMVECTOR v = XMLoadFloat3(&m_Vertices[i].Pos);
		v = XMVector3Transform(v, m_World);

		XMStoreFloat3(&float3, v);
		vec.push_back(float3);
	}

	m_AABB.CalculateAABB(vec);
}

void PickingBox::Render()
{	
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());

	//AABB는 PickingBox 내부에서 따로 돌아가는 애라서 예외로 이렇게 처리한다. 
	//테두리 선 렌더링 안할거면 지워도 됨
	m_AABB.SetupConstant();
	m_AABB.Render();
}

void PickingBox::InitResource(ObjectResource* resource)
{
	InitBuffer(resource);		
}

void PickingBox::InitBuffer(ObjectResource* resource)
{
	SimpleVertex vertices[] =
	{
		XMFLOAT3(-1.0f, 1.0f, -1.0f),
		XMFLOAT3(1.0f, 1.0f, -1.0f),
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(-1.0f, 1.0f, 1.0f),
		XMFLOAT3(-1.0f, -1.0f, -1.0f),
		XMFLOAT3(1.0f, -1.0f, -1.0f),
		XMFLOAT3(1.0f, -1.0f, 1.0f),
		XMFLOAT3(-1.0f, -1.0f, 1.0f)
	};

	for each (auto var in vertices)
	{
		m_Vertices.push_back(var);
	}

	m_Color = XMFLOAT4(1, 0, 0, 1);

	//AABB 초기화-------------------
	m_AABB.Init();
	m_AABB.SetColor({ 0, 0, 1, 1 });
	//-----------------------------

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;		//클릭 되면 색상 변경해줘야 하므로 Dynamic
	bd.ByteWidth = sizeof(SimpleVertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	wrapDevice->GetDevice()->CreateBuffer(&bd, &InitData, &resource->pVertexBuffer);

	WORD indices[] =
	{
		3, 1, 0,
		2, 1, 3,
		0, 5, 4,
		1, 5, 0,
		3, 4, 7,
		0, 4, 3,
		1, 6, 5,
		2, 6, 1,
		2, 7, 6,
		3, 7, 2,
		6, 4, 5,
		7, 4, 6
	};

	resource->indexNum = sizeof(indices) / sizeof(WORD);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * resource->indexNum;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	wrapDevice->GetDevice()->CreateBuffer(&bd, &InitData, &resource->pIndexBuffer);	
}

void PickingBox::Cleanup(){}

void PickingBox::SetupConstant()
{
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());	
}

void PickingBox::Pick(int sx, int sy)
{
	Camera& camera = Camera::GetInstance();
	XMMATRIX matProj = camera.Proj();

	// 스크린 공간 -> 뷰공간 변환
	float vx = (+2.0f * sx / WINDOW_WIDTH - 1.0f) / matProj(0, 0);
	float vy = (-2.0f * sy / WINDOW_HEIGHT + 1.0f) / matProj(1, 1);

	// 뷰 공간에서 카메라로 부터의 광선은 무조건 위치는 (0,0,0) 방향은 (0,0,1)
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	//AABB와 충돌비교를 위해 뷰->월드 공간 변경
	XMMATRIX matView = camera.View();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(matView), matView);

	rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
	rayDir = XMVector3TransformNormal(rayDir, invView);
	rayDir = XMVector3Normalize(rayDir);

	//Slab 방식의 Ray/Box 충돌비교
	if (m_AABB.IntersectRayAxisAlignedBox(rayOrigin, rayDir))
	{
		m_Color = { Util::RandF(0, 1), Util::RandF(0, 1), Util::RandF(0, 1), 1 };
	}
}

const RenderInfo* PickingBox::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(SimpleVertex);
	m_RenderInfo.offset = 0;

	return &m_RenderInfo;
}

const ConstantInfo* PickingBox::GetConstantInfo()
{	
	m_ConstantInfo.world = m_World;
	m_ConstantInfo.color = m_Color;
	return &m_ConstantInfo;
	
}

#pragma endregion PickingBox
