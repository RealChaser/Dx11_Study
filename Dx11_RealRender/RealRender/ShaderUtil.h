#pragma once

class ShaderUtil
{
public:
	ShaderUtil();
	~ShaderUtil();

public:		
	static bool CreateVertexShader(ID3D11Device* pDevice, LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppVSBlob, ID3D11VertexShader** ppVShader);
	static bool CreateHullShader(ID3D11Device* pDevice, LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppHSBlob, ID3D11HullShader** ppHShader);
	static bool CreateDomainShader(ID3D11Device* pDevice, LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppDSBlob, ID3D11DomainShader** ppDShader);
	static bool CreateGeometryShader(ID3D11Device* pDevice, LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppGSBlob, ID3D11GeometryShader** ppGShader);
	static bool CreatePixelShader(ID3D11Device* pDevice, LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppPSBlob, ID3D11PixelShader** ppPShader);	
private:
	static bool BuildShader( LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderMode, ID3DBlob** ppBlob );
};

