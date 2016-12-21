#pragma once

class DeviceDx11;
class Util
{
public:
	static ID3D11ShaderResourceView* CreateTexture2DArraySRV(DeviceDx11* pDevice,
															 vector<wstring>& filenames,
															 DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE,
															 UINT filter = D3DX11_FILTER_NONE,
															 UINT mipFilter = D3DX11_FILTER_LINEAR);

	static float RandF()	{	return (float) (rand()) / (float) RAND_MAX;		}
	static float RandF(float a, float b)	{	return a + RandF()*(b - a);		}
};

