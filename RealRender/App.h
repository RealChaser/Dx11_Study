#pragma once

#include "ShareValue.h"

class Camera;
class DeviceDx11;
class Lighting;
class Object;
class RenderManager;
class App
{
public:
	App();
	~App();	

public:
	bool Init(HWND hWnd);
	void Update(float dt);
	void Render();
	void CleanupApp();

public:
	void SetScreenPoint(int x, int y);
	
private:	
	void InitResource();
	void InitRenderer();
	void InitLight();
	void InitObejct();

private:
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;
	vector<Object*> m_vecObjects;
	
	Lighting* m_pLighting;

private:
	DeviceDx11* m_pDeviceDx11;
	RenderManager* m_pRenderManager;
};