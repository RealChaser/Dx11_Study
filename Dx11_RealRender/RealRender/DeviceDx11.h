#pragma once
#include "Singleton.h"
#include "ShareValue.h"

class DeviceDx11 : public Singleton<DeviceDx11>
{
	friend class Singleton<DeviceDx11>;
private:
	DeviceDx11() = default;
	DeviceDx11( const DeviceDx11& ) = delete;
	DeviceDx11& operator=( const DeviceDx11& ) = delete;

public:
	~DeviceDx11();

public:
	HRESULT InitDevice( HWND hWnd );	
	inline ID3D11Device* GetDevice()						{ return m_pd3dDevice; };
	inline IDXGISwapChain* GetSwapChain()					{ return m_pSwapChain; }
	inline ID3D11DeviceContext* GetDeviceContext()			{ return m_pImmediateContext; }	
	void CleanupDevice();	

private:
	D3D_FEATURE_LEVEL       m_featureLevel;
	IDXGISwapChain*         m_pSwapChain;
	ID3D11Device*           m_pd3dDevice;
	ID3D11DeviceContext*    m_pImmediateContext;	
};