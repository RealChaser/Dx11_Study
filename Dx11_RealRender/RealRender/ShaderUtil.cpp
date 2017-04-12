#include "stdafx.h"
#include "ShaderUtil.h"
#include "TemplateUtil.h"

ShaderUtil::ShaderUtil()
{
}


ShaderUtil::~ShaderUtil()
{	
}

bool ShaderUtil::BuildShader( LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlob )
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )	
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlob, &pErrorBlob, NULL );

	if( FAILED( hr ) )
	{
		if( pErrorBlob != NULL )
			OutputDebugStringA( ( char* )pErrorBlob->GetBufferPointer() );

		Safe_Release( pErrorBlob );

		return false;
	}

	Safe_Release( pErrorBlob );		
		
	if( FAILED( hr ) )
	{
		MessageBox( NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );	
		return false;
	}

	return true;
}

bool ShaderUtil::CreateVertexShader(ID3D11Device* pDevice, LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppVSBlob, ID3D11VertexShader** ppVShader)
{
	if( pDevice )
	{
		if( BuildShader( szFileName, szEntryPoint, szShaderModel, ppVSBlob ) )
		{				
			if (SUCCEEDED(pDevice->CreateVertexShader((*ppVSBlob)->GetBufferPointer(), (*ppVSBlob)->GetBufferSize(), NULL, ppVShader)))
			{
				return true;
			}			
		}
	}

	return false;	
}

bool ShaderUtil::CreatePixelShader(ID3D11Device* pDevice, LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppPSBlob, ID3D11PixelShader** ppPShader)
{
	if( pDevice )
	{
		if( BuildShader( szFileName, szEntryPoint, szShaderModel, ppPSBlob ) )
		{
			if (SUCCEEDED(pDevice->CreatePixelShader((*ppPSBlob)->GetBufferPointer(), (*ppPSBlob)->GetBufferSize(), NULL, ppPShader)))
			{
				return true;
			}
		}
	}

	return false;
}

bool ShaderUtil::CreateHullShader(ID3D11Device* pDevice, LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppHSBlob, ID3D11HullShader** ppHShader)
{
	if (pDevice)
	{
		if (BuildShader(szFileName, szEntryPoint, szShaderModel, ppHSBlob))
		{
			if (SUCCEEDED(pDevice->CreateHullShader((*ppHSBlob)->GetBufferPointer(), (*ppHSBlob)->GetBufferSize(), NULL, ppHShader)))
			{
				return true;
			}
		}
	}

	return false;
}

bool ShaderUtil::CreateDomainShader(ID3D11Device* pDevice, LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppDSBlob, ID3D11DomainShader** ppDShader)
{
	if (pDevice)
	{
		if (BuildShader(szFileName, szEntryPoint, szShaderModel, ppDSBlob))
		{
			if (SUCCEEDED(pDevice->CreateDomainShader((*ppDSBlob)->GetBufferPointer(), (*ppDSBlob)->GetBufferSize(), NULL, ppDShader)))
			{
				return true;
			}
		}
	}

	return false;
}

bool ShaderUtil::CreateGeometryShader(ID3D11Device* pDevice, LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppGSBlob, ID3D11GeometryShader** ppGShader)
{
	if (pDevice)
	{
		if (BuildShader(szFileName, szEntryPoint, szShaderModel, ppGSBlob))
		{
			if (SUCCEEDED(pDevice->CreateGeometryShader((*ppGSBlob)->GetBufferPointer(), (*ppGSBlob)->GetBufferSize(), NULL, ppGShader)))
			{
				return true;
			}
		}
	}

	return false;
}
