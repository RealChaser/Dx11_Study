#include "stdafx.h"
#include "DeviceDx11.h"
#include "TemplateUtil.h"

DeviceDx11::~DeviceDx11()
{
	CleanupDevice();
}

HRESULT DeviceDx11::InitDevice( HWND hWnd )
{	
	m_featureLevel		= D3D_FEATURE_LEVEL_11_0;
	m_pd3dDevice		= NULL;
	m_pImmediateContext = NULL;
	m_pSwapChain		= NULL;

	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect( hWnd, &rc );
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SampleDesc.Count = 1;	
	sd.SampleDesc.Quality = 0;	
	
	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{		
		D3D_DRIVER_TYPE driverType = driverTypes[ driverTypeIndex ];
		hr = D3D11CreateDeviceAndSwapChain( NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext );

		if( SUCCEEDED( hr ) )
			break;
	}

	return hr;	
}

void DeviceDx11::CleanupDevice()
{
	Safe_Release( m_pImmediateContext );		
	Safe_Release( m_pSwapChain );
	Safe_Release( m_pd3dDevice );	//디바이스는 모든 Com객체가 Release된 후에 Release해야 한다.
}