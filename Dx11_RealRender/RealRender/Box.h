#pragma once
#include "Object.h"

////////////////////////////////////////////////////////
// Box
////////////////////////////////////////////////////////
#pragma region Box
class Box : public Object
{
public:
	Box();
	virtual ~Box();

	ALIGNED16(Box);

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
#pragma endregion Box

////////////////////////////////////////////////////////
// LightBox
////////////////////////////////////////////////////////
#pragma region LightBox
class LightBox : public Box
{
public:
	LightBox();
	virtual ~LightBox();

	ALIGNED16(LightBox);

public:
	virtual void Init();
	virtual void Update(float dt);
	virtual void Render();
	virtual void Cleanup();

protected:
	virtual void InitResource(ObjectResource* resource);			

public:
	virtual const RenderInfo* GetRenderInfo();
	virtual const ConstantInfo* GetConstantInfo();
	virtual void SetupConstant();
};
#pragma endregion LightBox

////////////////////////////////////////////////////////
// TextureBox
////////////////////////////////////////////////////////
#pragma region TextureBox
class TextureBox : public Box
{
public:
	TextureBox();
	virtual ~TextureBox();

	ALIGNED16(TextureBox);

public:
	virtual void Init();
	virtual void Update(float dt);
	virtual void Render();
	virtual void Cleanup();

protected:
	virtual void InitResource(ObjectResource* resource);		
	virtual void InitTexture(ObjectResource* resource);		

public:
	virtual const RenderInfo* GetRenderInfo();
	virtual const ConstantInfo* GetConstantInfo();
	virtual void SetupConstant();
};
#pragma endregion TextureBox

////////////////////////////////////////////////////////
// InstanceBox
////////////////////////////////////////////////////////
#pragma region InstanceBox
class InstanceBox : public Box
{
public:
	InstanceBox();
	virtual ~InstanceBox();

	ALIGNED16(InstanceBox);

public:
	virtual void Init();
	virtual void Update(float dt);
	virtual void Render();
	virtual void Cleanup();

protected:
	virtual void InitResource(ObjectResource* resource);			

public:
	virtual const RenderInfo* GetRenderInfo();
	virtual const ConstantInfo* GetConstantInfo();
	virtual void SetupConstant();

private:
	void InitInstance(ObjectResource* resource);

private:
	vector<InstanceInfo> m_instancedData;
	UINT m_visibleObjectCount;
};
#pragma endregion InstanceBox

////////////////////////////////////////////////////////
// AABB
////////////////////////////////////////////////////////
#pragma region AABB
class AABB : public Box
{
public:
	AABB();
	virtual ~AABB();

	ALIGNED16(AABB);

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

public:
	void CalculateAABB(vector<XMFLOAT3> vertices);
	void SetColor(XMFLOAT4 color);
	bool IntersectRayAxisAlignedBox(XMVECTOR rayPos, XMVECTOR rayDir);

private:
	void SetMinMax(XMFLOAT3 min, XMFLOAT3 max);
	XMFLOAT3 GetMin(vector<XMFLOAT3> vertices);
	XMFLOAT3 GetMax(vector<XMFLOAT3> vertices);

private:
	XMMATRIX m_BoxWorld; //m_World는 그려질 때 사용되기 때문에 m_BoxWorld를 따로 둬서 월드값을 저장
	XMFLOAT4 m_Color;
	array<SimpleVertex, 8> m_Vertices;
	ID3D11RasterizerState* m_WireframeRS;
};
#pragma endregion AABB

////////////////////////////////////////////////////////
// PickingBox
////////////////////////////////////////////////////////
#pragma region PickingBox
class PickingBox : public Box
{
public:
	PickingBox();
	virtual ~PickingBox();

	ALIGNED16(PickingBox);

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

public:
	void Pick(int sx, int sy);

private:
	vector<SimpleVertex> m_Vertices;
	XMFLOAT4 m_Color;
	AABB m_AABB;
};
#pragma endregion PickingBox;