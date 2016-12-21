#pragma once
#include "Object.h"

////////////////////////////////////////////////////////
// Sphere
////////////////////////////////////////////////////////
#pragma region Sphere
class Sphere : public Object
{
public:
	Sphere();
	virtual ~Sphere();

	ALIGNED16(Sphere);

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
	virtual void  SetupConstant();

public:
	inline void SetRadius(float radius){ m_Radius = radius; }
	inline void SetSliceCount(int sliceCount){ m_SliceCount = sliceCount; }
	inline void SetStackCount(int stackCount){ m_StackCount = stackCount; }	

protected:
	float m_Radius;	// 구의 반경
	int m_SliceCount;	// 
	int m_StackCount;
};
#pragma endregion Sphere

////////////////////////////////////////////////////////
// LightSphere
////////////////////////////////////////////////////////
#pragma region LightSphere
class LightSphere : public Sphere
{
public:
	LightSphere();
	virtual ~LightSphere();

	ALIGNED16(LightSphere);

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
#pragma endregion LightSphere

////////////////////////////////////////////////////////
// ReflectSphere
////////////////////////////////////////////////////////
#pragma region ReflectSphere
class ReflectSphere : public LightSphere
{
public:
	ReflectSphere(){};
	virtual ~ReflectSphere(){};

	ALIGNED16(ReflectSphere);

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
#pragma endregion ReflectSphere

////////////////////////////////////////////////////////
// DisplacementSphere
////////////////////////////////////////////////////////
#pragma region DisplacementSphere
class DisplacementSphere : public LightSphere
{
public:
	DisplacementSphere(){};
	virtual ~DisplacementSphere(){};

	ALIGNED16(DisplacementSphere);

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
#pragma endregion DisplacementSphere