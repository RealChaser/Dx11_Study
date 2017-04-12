#pragma once
#include "ShareValue.h"

class DeviceDx11;
class Lighting
{
public:
	Lighting();
	~Lighting();

public:	
	void Init();
	void Update();
	void Adjust();
	void CleanupApp();

private:
	int m_constantInfoSize;
	ID3D11Buffer* m_lightConstantBuffer;	
	DirLightBuffer m_lightConstantInfo;

private:
	DeviceDx11* wrapDevice;

public:
	void SetDevice(DeviceDx11* pDevice){ wrapDevice = pDevice; }
};
