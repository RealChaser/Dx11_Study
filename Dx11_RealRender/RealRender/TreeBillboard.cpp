#include "stdafx.h"
#include "TreeBillboard.h"
#include "ShareValue.h"
#include "DeviceDx11.h"
#include "TemplateUtil.h"
#include "ShaderUtil.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Util.h"
#include "RenderManager.h"

TreeBillboard::TreeBillboard()
{}

TreeBillboard::~TreeBillboard()
{
	Cleanup();
}

void TreeBillboard::Init()
{
	m_Mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_Mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Mat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_Mat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_ObjectName = "TreeBillboard";
	m_RenderKey = RENDER_KEY::BILLBOARD;
	Object::Init();
}

void TreeBillboard::Update(float dt){}

void TreeBillboard::Render()
{
	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void TreeBillboard::Cleanup()
{}

void TreeBillboard::InitResource(ObjectResource* resource)
{
	InitBuffer(resource);
	InitTexture(resource);	
}

void TreeBillboard::InitBuffer(ObjectResource* resource)
{
	TreeBillboardVertex v;	
	
	v.Pos = m_Position;
	v.Size = m_Size;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(TreeBillboardVertex);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &v;
	wrapDevice->GetDevice()->CreateBuffer(&bd, &vinitData, &resource->pVertexBuffer);
}

void TreeBillboard::InitTexture(ObjectResource* resource)
{
	vector<wstring> treeFilenames;
	treeFilenames.push_back(L"Textures\\tree0.dds");
	treeFilenames.push_back(L"Textures\\tree1.dds");
	treeFilenames.push_back(L"Textures\\tree2.dds");
	treeFilenames.push_back(L"Textures\\tree3.dds");

	ID3D11ShaderResourceView* pTextureSRV = nullptr;
	pTextureSRV = Util::CreateTexture2DArraySRV(wrapDevice, treeFilenames);
	RenderManager::GetInstance().AddTextureSRV(m_RenderKey, pTextureSRV);
}

void TreeBillboard::SetupConstant()
{	
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

const RenderInfo* TreeBillboard::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(TreeBillboardVertex);
	m_RenderInfo.offset = 0;
	m_RenderInfo.primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	return &m_RenderInfo;
}

const ConstantInfo* TreeBillboard::GetConstantInfo()
{
	m_ConstantInfo.world = m_World;
	m_ConstantInfo.material = m_Mat;

	return &m_ConstantInfo;
}

