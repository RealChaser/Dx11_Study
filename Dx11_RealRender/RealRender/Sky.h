#pragma once
#include "Object.h"

class Sky :	public Object
{
public:
	Sky();
	virtual ~Sky();

	ALIGNED16(Sky);

public:
	virtual void Init();
	virtual void Update(float dt);
	virtual void Render();
	virtual void Cleanup();

protected:
	virtual void InitResource(ObjectResource* resource);
	virtual void InitBuffer(ObjectResource* resource);	
	virtual void InitTexture(ObjectResource* resource);	

public:
	virtual const RenderInfo* GetRenderInfo();
	virtual const ConstantInfo* GetConstantInfo();
	virtual void SetupConstant();

private:
	ID3D11RasterizerState* m_SkyRS;
	ID3D11DepthStencilState* m_SkyDSS;
};

