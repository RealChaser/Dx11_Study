#pragma once
#include "Object.h"

class ShadowDepth :	public Object
{
public:
	ShadowDepth();
	virtual ~ShadowDepth();

	ALIGNED16(ShadowDepth);

public:
	virtual void Init();
	virtual void Update(float dt);
	virtual void Render();
	virtual void Cleanup();

protected:
	virtual void InitResource(ObjectResource* resource);
	virtual void InitBuffer(ObjectResource* resource);	

public:
	virtual const RenderInfo* GetRenderInfo();
	virtual const ConstantInfo* GetConstantInfo();
	virtual void SetupConstant();	
};