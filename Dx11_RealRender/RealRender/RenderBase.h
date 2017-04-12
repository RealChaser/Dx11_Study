#pragma once
#include "ShareValue.h"

class DeviceDx11;
class RenderBase
{
public:
	RenderBase();
	virtual ~RenderBase();

public:
	virtual void InitShader() = 0;
	virtual void InitConstant() = 0;
	virtual void InitSampler(){}
	virtual void InitEtc(){}
	virtual void AddTextureSRV(ID3D11ShaderResourceView* pTextureSRV){ m_vecTextureSRV.push_back(pTextureSRV); }
	virtual void SetupConstant(const ConstantInfo* info) = 0;
	virtual void Render(ObjectResource* pResource, const RenderInfo* info) = 0;

public:
	RENDER_KEY GetRenderKey(){ return m_Renderkey; }

protected:
	wstring					m_ShaderName;
	RENDER_KEY				m_Renderkey;
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11GeometryShader*	m_pGeometryShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11HullShader*		m_pHullShader;
	ID3D11DomainShader*		m_pDomainShader;	
	vector<ID3D11ShaderResourceView*> m_vecTextureSRV;
	map<string, ID3D11SamplerState*> m_mapSampler;
	map<string, ID3D11Buffer*> m_mapConstant;

protected:
	static DeviceDx11* wrapDevice;

public:
	static void SetDevice(DeviceDx11* pDevice){ wrapDevice = pDevice; }
};

class SolidShaderRender : public RenderBase
{
public:
	SolidShaderRender();
	virtual ~SolidShaderRender(){}

public:
	virtual void InitShader();
	virtual void InitConstant();
	virtual void SetupConstant(const ConstantInfo* info);
	virtual void Render(ObjectResource* pResource, const RenderInfo* info);
};

class LightShaderRender : public RenderBase
{
public:
	LightShaderRender();
	virtual ~LightShaderRender(){}

public:
	virtual void InitShader();
	virtual void InitConstant();
	virtual void SetupConstant(const ConstantInfo* info);
	virtual void Render(ObjectResource* pResource, const RenderInfo* info);
};

class TextureShaderRender : public RenderBase
{
public:
	TextureShaderRender();
	virtual ~TextureShaderRender(){};

public:
	virtual void InitShader();
	virtual void InitSampler();	
	virtual void InitConstant();
	virtual void SetupConstant(const ConstantInfo* info);
	virtual void Render(ObjectResource* pResource, const RenderInfo* info);
};

class ReflectShaderRender : public RenderBase
{
public:
	ReflectShaderRender();
	virtual ~ReflectShaderRender(){};

public:
	virtual void InitShader();
	virtual void InitSampler();
	virtual void InitConstant();
	virtual void SetupConstant(const ConstantInfo* info);
	virtual void Render(ObjectResource* pResource, const RenderInfo* info);
};

class DisplacementShaderRender : public RenderBase
{
public:
	DisplacementShaderRender();
	virtual ~DisplacementShaderRender(){};

public:
	virtual void InitShader();
	virtual void InitSampler();
	virtual void InitConstant();
	virtual void SetupConstant(const ConstantInfo* info);
	virtual void Render(ObjectResource* pResource, const RenderInfo* info);
};

class SkyShaderRender : public RenderBase
{
public:
	SkyShaderRender();
	virtual ~SkyShaderRender();

public:
	virtual void InitShader();
	virtual void InitSampler();
	virtual void InitConstant();
	virtual void InitEtc();	
	virtual void SetupConstant(const ConstantInfo* info);
	virtual void Render(ObjectResource* pResource, const RenderInfo* info);

private:
	ID3D11RasterizerState* m_SkyRS;
	ID3D11DepthStencilState* m_SkyDSS;
};

class TerrainShaderRender : public RenderBase
{	
public:
	TerrainShaderRender();
	virtual ~TerrainShaderRender(){};

public:
	virtual void InitShader();
	virtual void InitSampler();
	virtual void InitConstant();
	virtual void SetupConstant(const ConstantInfo* info);
	virtual void Render(ObjectResource* pResource, const RenderInfo* info);

private:
	void ExtractFrustumPlanes(XMFLOAT4 planes[6], CXMMATRIX M);
};

class InstanceShaderRender : public RenderBase
{
public:
	InstanceShaderRender();
	virtual ~InstanceShaderRender(){};

public:
	virtual void InitShader();	
	virtual void InitConstant();
	virtual void SetupConstant(const ConstantInfo* info);
	virtual void Render(ObjectResource* pResource, const RenderInfo* info);
};

class BillboardShaderRender : public RenderBase
{
public:
	BillboardShaderRender();
	virtual ~BillboardShaderRender();

public:
	virtual void InitShader();
	virtual void InitSampler();
	virtual void InitConstant();
	virtual void InitEtc();
	virtual void SetupConstant(const ConstantInfo* info);
	virtual void Render(ObjectResource* pResource, const RenderInfo* info);

private:
	ID3D11BlendState* m_AlphaToCoverageBS;
};