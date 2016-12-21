#pragma once
#include "Object.h"

class TreeBillboard : public Object
{
public:
	TreeBillboard();
	virtual ~TreeBillboard();

	ALIGNED16(TreeBillboard);

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

public:
	void SetSize(XMFLOAT2 size){ m_Size = size; }

private:
	XMFLOAT2 m_Size;
	Material m_Mat;
	
};

