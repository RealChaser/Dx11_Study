#pragma once
#include "Singleton.h"
#include "ShareValue.h"

class RenderBase;
class RenderManager : public Singleton < RenderManager >
{
	friend class Singleton < RenderManager >;

private:
	vector<RenderBase*> m_vecRender;

public:
	void AddRender(RenderBase* pNewRenderBase);

	void SetupConstant(RENDER_KEY renderKey, const ConstantInfo* info);
	void AddTextureSRV(RENDER_KEY renderKey, ID3D11ShaderResourceView* pTextureSRV);
	void Render(RENDER_KEY renderKey, ObjectResource* pResource, const RenderInfo* info);

public:
	RenderManager();
	~RenderManager();
};
