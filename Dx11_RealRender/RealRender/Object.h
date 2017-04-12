#pragma once
#include "ShareValue.h"

class DeviceDx11;
class ShaderUtil;
class Object
{
public:
	Object();
	virtual ~Object();

	ALIGNED16(Object);

public:
	virtual void Init();
	virtual void Update(float dt) = 0;
	virtual void Render();
	virtual void Cleanup();

protected:
	virtual void InitResource(ObjectResource* resource) = 0;	
	virtual void InitBuffer(ObjectResource* resource){};	
	virtual void InitTexture(ObjectResource* resource){};		

public:	
	virtual const RenderInfo* GetRenderInfo() = 0;
	virtual const ConstantInfo* GetConstantInfo() = 0;
	virtual void SetupConstant() = 0;
	
public:
	inline string& GetObjectName(){ return m_ObjectName; }
	inline RENDER_KEY GetRenderKey(){ return m_RenderKey; }

	inline XMMATRIX GetWorld(){ return m_World; }
	inline void SetPosition(XMFLOAT3 position){ m_Position = position; }

protected:
	XMMATRIX			m_World;
	XMFLOAT3			m_Position;
	string				m_ObjectName;
	RENDER_KEY			m_RenderKey;
	RenderInfo			m_RenderInfo;
	ConstantInfo		m_ConstantInfo;

protected:
	static DeviceDx11* wrapDevice;
public:
	static void SetDevice(DeviceDx11* pDevice){ wrapDevice = pDevice; }
};
