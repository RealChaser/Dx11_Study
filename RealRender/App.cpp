#include "stdafx.h"
#include "App.h"
#include "TemplateUtil.h"
#include "DeviceDx11.h"
#include "ShaderUtil.h"
#include "ShareValue.h"
#include "Object.h"
#include "Camera.h"
#include "Sky.h"
#include "Lighting.h"
#include "Terrain.h"
#include "TreeBillboard.h"
#include "Sphere.h"
#include "Box.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "RenderBase.h"

App::App()
	: m_pRenderTargetView(nullptr), m_pDepthStencilView(nullptr), m_pLighting(nullptr), m_pDeviceDx11(nullptr)
{
}

App::~App()
{
	CleanupApp();
}

bool App::Init( HWND hWnd )
{	
	m_pDeviceDx11 = &DeviceDx11::GetInstance();
	m_pRenderManager = &RenderManager::GetInstance();

	if (m_pDeviceDx11->InitDevice(hWnd))
		return false;

	InitRenderer();
	InitResource();	

	InitLight();
	InitObejct();

	return true;
}

void App::InitResource()
{			 
	ID3D11Texture2D* pTexture2D = nullptr;
	if (SUCCEEDED(m_pDeviceDx11->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture2D)))
		&& SUCCEEDED(m_pDeviceDx11->GetDevice()->CreateRenderTargetView(pTexture2D, 0, &m_pRenderTargetView)))
	{
		Safe_Release(pTexture2D);			
	}
		
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = WINDOW_WIDTH;
	depthStencilDesc.Height = WINDOW_HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	if (SUCCEEDED(m_pDeviceDx11->GetDevice()->CreateTexture2D(&depthStencilDesc, 0, &pTexture2D))
		&& SUCCEEDED(m_pDeviceDx11->GetDevice()->CreateDepthStencilView(pTexture2D, 0, &m_pDepthStencilView)))
	{
		Safe_Release(pTexture2D);
	}
		
	m_pDeviceDx11->GetDeviceContext()->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_VIEWPORT screenViewport;
	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	screenViewport.Width = static_cast<float>(WINDOW_WIDTH);
	screenViewport.Height = static_cast<float>(WINDOW_HEIGHT);
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;

	m_pDeviceDx11->GetDeviceContext()->RSSetViewports(1, &screenViewport);
}

void App::Update(float dt)
{	
	m_pLighting->Update();

	for each (Object* pObj in m_vecObjects)
	{
		pObj->Update(dt);
	}
}

void App::Render()
{
	Camera& camera = Camera::GetInstance();
	 
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	m_pDeviceDx11->GetDeviceContext()->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
	m_pDeviceDx11->GetDeviceContext()->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
	m_pLighting->Adjust();	//광원 적용
		
	for each (Object* pObj in m_vecObjects)
	{
		pObj->SetupConstant();
		pObj->Render();
	}

	m_pDeviceDx11->GetSwapChain()->Present(0, 0);
}

void App::CleanupApp()
{	
	Safe_Delete(m_pLighting);
	Safe_Delete_VecList(m_vecObjects);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pRenderTargetView);
}

void App::SetScreenPoint(int x, int y)
{	//클릭했을 때 충돌체크 관련 객체 여부 판단 및 진행
	for each (auto pObj in m_vecObjects)
	{
		if (PickingBox* pPickingBox = dynamic_cast<PickingBox*>(pObj))
		{
			pPickingBox->Pick(x, y);			
		}
	}
}

void App::InitRenderer()
{
	RenderBase::SetDevice(m_pDeviceDx11);

	SolidShaderRender* pSolidRender = new SolidShaderRender;
	pSolidRender->InitShader();
	pSolidRender->InitConstant();
	m_pRenderManager->AddRender(pSolidRender);

	LightShaderRender* pLightRender = new LightShaderRender;
	pLightRender->InitShader();
	pLightRender->InitConstant();
	m_pRenderManager->AddRender(pLightRender);

	TextureShaderRender* pTextureRender = new TextureShaderRender;
	pTextureRender->InitShader();
	pTextureRender->InitConstant();
	pTextureRender->InitSampler();
	m_pRenderManager->AddRender(pTextureRender);

	ReflectShaderRender* pReflectRender = new ReflectShaderRender;
	pReflectRender->InitShader();
	pReflectRender->InitConstant();
	pReflectRender->InitSampler();
	m_pRenderManager->AddRender(pReflectRender);

	DisplacementShaderRender* pDisplacementRender = new DisplacementShaderRender;
	pDisplacementRender->InitShader();
	pDisplacementRender->InitConstant();
	pDisplacementRender->InitSampler();
	m_pRenderManager->AddRender(pDisplacementRender);

	SkyShaderRender* pSkyRender = new SkyShaderRender;
	pSkyRender->InitShader();
	pSkyRender->InitSampler();
	pSkyRender->InitConstant();
	pSkyRender->InitEtc();
	m_pRenderManager->AddRender(pSkyRender);

	TerrainShaderRender* pTerrainRender = new TerrainShaderRender;
	pTerrainRender->InitShader();
	pTerrainRender->InitSampler();
	pTerrainRender->InitConstant();
	m_pRenderManager->AddRender(pTerrainRender);

	InstanceShaderRender* pInstanceRender = new InstanceShaderRender;
	pInstanceRender->InitShader();
	pInstanceRender->InitConstant();
	m_pRenderManager->AddRender(pInstanceRender);

	BillboardShaderRender* pBillboardRender = new BillboardShaderRender;
	pBillboardRender->InitShader();
	pBillboardRender->InitSampler();
	pBillboardRender->InitConstant();
	pBillboardRender->InitEtc();
	m_pRenderManager->AddRender(pBillboardRender);
}

void App::InitObejct()
{
	Object::SetDevice(m_pDeviceDx11);

	//오브젝트 셋팅
	Object* pObj = new Box();
	pObj->Init();
	pObj->SetPosition({ -10, 5, 150 });
	m_vecObjects.push_back(pObj);

	pObj = new PickingBox();
	pObj->Init();
	pObj->SetPosition({ 3, 5, 150 });
	m_vecObjects.push_back(pObj);

	pObj = new LightBox();
	pObj->SetPosition({ 16, 5, 150 });
	pObj->Init();
	m_vecObjects.push_back(pObj);

	pObj = new TextureBox();
	pObj->SetPosition({ 29, 5, 150 });
	pObj->Init();
	m_vecObjects.push_back(pObj);

	Sphere* pSphere1 = new Sphere();
	pSphere1->SetPosition({ -8, 30, 150 });
	pSphere1->SetStackCount(40);
	pSphere1->SetSliceCount(40);
	pSphere1->SetRadius(5);
	pSphere1->Init();
	m_vecObjects.push_back(pSphere1);

	LightSphere* pSphere = new LightSphere();
	pSphere->SetPosition({ 5, 30, 150 });
	pSphere->SetStackCount(40);
	pSphere->SetSliceCount(40);
	pSphere->SetRadius(5);
	pSphere->Init();
	m_vecObjects.push_back(pSphere);

	ReflectSphere* pRSphere = new ReflectSphere();
	pRSphere->SetPosition({ 18, 30, 150 });
	pRSphere->SetStackCount(40);
	pRSphere->SetSliceCount(40);
	pRSphere->SetRadius(5);
	pRSphere->Init();
	m_vecObjects.push_back(pRSphere);

	DisplacementSphere* pDSphere = new DisplacementSphere();
	pDSphere->SetPosition({ 31, 30, 150 });
	pDSphere->SetStackCount(40);
	pDSphere->SetSliceCount(40);
	pDSphere->SetRadius(5);
	pDSphere->Init();
	m_vecObjects.push_back(pDSphere);

	pObj = new Sky();
	pObj->Init();
	m_vecObjects.push_back(pObj);

	Terrain* pTerrain = new Terrain();
	pTerrain->Init();
	m_vecObjects.push_back(pTerrain);

	pObj = new InstanceBox();
	pObj->SetPosition({ -45, 20, 150 });
	pObj->Init();
	m_vecObjects.push_back(pObj);

	TreeBillboard* pTreeBillboard = new TreeBillboard();
	float x = 50.f;
	float z = 150.f;
	pTreeBillboard->SetPosition({ x, pTerrain->GetHeight(x, z) + 8.0f, z });
	pTreeBillboard->SetSize({ 20.f, 20.f });
	pTreeBillboard->Init();
	m_vecObjects.push_back(pTreeBillboard);
}

void App::InitLight()
{
	//광원 초기화
	m_pLighting = new Lighting();
	m_pLighting->SetDevice(m_pDeviceDx11);
	m_pLighting->Init();
}