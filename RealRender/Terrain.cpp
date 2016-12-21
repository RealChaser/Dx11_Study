#include "stdafx.h"
#include "Terrain.h"
#include "ShareValue.h"
#include "DeviceDx11.h"
#include "TemplateUtil.h"
#include "ShaderUtil.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Util.h"
#include "RenderManager.h"

const float TERRAIN_INFINITY = 3.402823466e+38F;

Terrain::Terrain() 
	:m_NumPatchVertices(0), m_NumPatchQuadFaces(0), m_NumPatchVertRows(0), m_NumPatchVertCols(0)
{
}

Terrain::~Terrain()
{
	Cleanup();
}

void Terrain::Init()
{	
	m_HeightMapFilename = L"Textures\\terrain.raw";
	m_LayerMapFilename0 = L"Textures\\grass.dds";
	m_LayerMapFilename1 = L"Textures\\darkdirt.dds";
	m_LayerMapFilename2 = L"Textures\\stone.dds";
	m_LayerMapFilename3 = L"Textures\\lightdirt.dds";
	m_LayerMapFilename4 = L"Textures\\snow.dds";
	m_BlendMapFilename = L"Textures\\blend.dds";

	m_HeightScale = 50.0f;
	m_HeightmapWidth = 2049;
	m_HeightmapHeight = 2049;
	m_CellSpacing = 0.5f;

	// Divide heightmap into patches such that each patch has CellsPerPatch.
	m_NumPatchVertRows = ((m_HeightmapHeight - 1) / CellsPerPatch) + 1;
	m_NumPatchVertCols = ((m_HeightmapWidth - 1) / CellsPerPatch) + 1;

	m_NumPatchVertices = m_NumPatchVertRows*m_NumPatchVertCols;
	m_NumPatchQuadFaces = (m_NumPatchVertRows - 1)*(m_NumPatchVertCols - 1);

	LoadHeightmap();
	Smooth();
	CalcAllPatchBoundsY();

	m_ObjectName = "Terrain";
	m_RenderKey = RENDER_KEY::TERRAIN;
	Object::Init();	;
}

void Terrain::Update(float dt)
{
}

void Terrain::Render()
{
	Object::Render();

	ObjectResource* pResource = ResourceManager::GetInstance().GetObjectResource(m_ObjectName);
	RenderManager::GetInstance().Render(m_RenderKey, pResource, GetRenderInfo());
}

void Terrain::Cleanup(){}

void Terrain::InitResource(ObjectResource* resource)
{
	InitBuffer(resource);
	InitTexture(resource);
}

void Terrain::InitBuffer(ObjectResource* resource)
{	
	std::vector<TerrainInfo> patchVertices(m_NumPatchVertRows * m_NumPatchVertCols);

	float halfWidth = 0.5f*GetWidth();
	float halfDepth = 0.5f*GetDepth();

	float patchWidth = GetWidth() / (m_NumPatchVertCols - 1);
	float patchDepth = GetDepth() / (m_NumPatchVertRows - 1);
	float du = 1.0f / (m_NumPatchVertCols - 1);
	float dv = 1.0f / (m_NumPatchVertRows - 1);

	for (UINT i = 0; i < m_NumPatchVertRows; ++i)
	{
		float z = halfDepth - i*patchDepth;
		for (UINT j = 0; j < m_NumPatchVertCols; ++j)
		{
			float x = -halfWidth + j*patchWidth;

			patchVertices[i*m_NumPatchVertCols + j].Pos = XMFLOAT3(x, 0.0f, z);

			// Stretch texture over grid.
			patchVertices[i*m_NumPatchVertCols + j].Tex.x = j*du;
			patchVertices[i*m_NumPatchVertCols + j].Tex.y = i*dv;
		}
	}

	// Store axis-aligned bounding box y-bounds in upper-left patch corner.
	for (UINT i = 0; i < m_NumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < m_NumPatchVertCols - 1; ++j)
		{
			UINT patchID = i*(m_NumPatchVertCols - 1) + j;
			patchVertices[i*m_NumPatchVertCols + j].BoundsY = m_PatchBoundsY[patchID];
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(TerrainInfo) * patchVertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &patchVertices[0];
	wrapDevice->GetDevice()->CreateBuffer(&vbd, &vinitData, &resource->pVertexBuffer);

	///////////////////////////////////
	//인덱스 버퍼
	std::vector<USHORT> indices(m_NumPatchQuadFaces * 4); // 4 indices per quad face

	// Iterate over each quad and compute indices.
	int k = 0;
	for (UINT i = 0; i < m_NumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < m_NumPatchVertCols - 1; ++j)
		{
			// Top row of 2x2 quad patch
			indices[k] = i * m_NumPatchVertCols + j;
			indices[k + 1] = i * m_NumPatchVertCols + j + 1;

			// Bottom row of 2x2 quad patch
			indices[k + 2] = (i + 1) * m_NumPatchVertCols + j;
			indices[k + 3] = (i + 1) * m_NumPatchVertCols + j + 1;

			k += 4; // next quad
		}
	}
	resource->indexNum = indices.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	wrapDevice->GetDevice()->CreateBuffer(&ibd, &iinitData, &resource->pIndexBuffer);	
}

void Terrain::InitTexture(ObjectResource* resource)
{	
	std::vector<std::wstring> layerFilenames;
	layerFilenames.push_back(m_LayerMapFilename0);
	layerFilenames.push_back(m_LayerMapFilename1);
	layerFilenames.push_back(m_LayerMapFilename2);
	layerFilenames.push_back(m_LayerMapFilename3);
	layerFilenames.push_back(m_LayerMapFilename4);

	ID3D11ShaderResourceView* pTextureSRV = nullptr;
		
	pTextureSRV = Util::CreateTexture2DArraySRV(wrapDevice, layerFilenames);
	RenderManager::GetInstance().AddTextureSRV(m_RenderKey, pTextureSRV);	//0번

	D3DX11CreateShaderResourceViewFromFile(wrapDevice->GetDevice(), m_BlendMapFilename.c_str(), 0, 0, &pTextureSRV, 0);
	RenderManager::GetInstance().AddTextureSRV(m_RenderKey, pTextureSRV);	//1번
	
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = m_HeightmapWidth;
	texDesc.Height = m_HeightmapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// HALF is defined in xnamath.h, for storing 16-bit float.
	std::vector<HALF> hmap(m_Heightmap.size());
	std::transform(m_Heightmap.begin(), m_Heightmap.end(), hmap.begin(), XMConvertFloatToHalf);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &hmap[0];
	data.SysMemPitch = m_HeightmapWidth*sizeof(HALF);
	data.SysMemSlicePitch = 0;

	ID3D11Texture2D* hmapTex = 0;
	wrapDevice->GetDevice()->CreateTexture2D(&texDesc, &data, &hmapTex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
		
	wrapDevice->GetDevice()->CreateShaderResourceView(hmapTex, &srvDesc, &pTextureSRV); //2
	RenderManager::GetInstance().AddTextureSRV(m_RenderKey, pTextureSRV);
	
	Safe_Release(hmapTex);
}

void Terrain::SetupConstant()
{
	RenderManager::GetInstance().SetupConstant(m_RenderKey, GetConstantInfo());
}

float Terrain::GetHeight(float x, float z) const
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f * GetWidth()) / m_CellSpacing;
	float d = (z - 0.5f * GetDepth()) / -m_CellSpacing;

	// Get the row and column we are in.
	int row = (int) floorf(d);
	int col = (int) floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = m_Heightmap[row*m_HeightmapWidth + col];
	float B = m_Heightmap[row*m_HeightmapWidth + col + 1];
	float C = m_Heightmap[(row + 1)*m_HeightmapWidth + col];
	float D = m_Heightmap[(row + 1)*m_HeightmapWidth + col + 1];

	// Where we are relative to the cell.
	float s = c - (float) col;
	float t = d - (float) row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
}

void Terrain::LoadHeightmap()
{
	// A height for each vertex
	std::vector<unsigned char> in(m_HeightmapWidth * m_HeightmapHeight);

	// Open the file.
	std::ifstream inFile;
	inFile.open(m_HeightMapFilename.c_str(), std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*) &in[0], (std::streamsize)in.size());
		inFile.close();
	}

	// Copy the array data into a float array and scale it.
	m_Heightmap.resize(m_HeightmapHeight * m_HeightmapWidth, 0);
	for (UINT i = 0; i < m_HeightmapHeight * m_HeightmapWidth; ++i)
	{
		m_Heightmap[i] = (in[i] / 255.0f)*m_HeightScale;
	}
}

void Terrain::Smooth()
{
	std::vector<float> dest(m_Heightmap.size());

	for (UINT i = 0; i < m_HeightmapHeight; ++i)
	{
		for (UINT j = 0; j < m_HeightmapWidth; ++j)
		{
			dest[i * m_HeightmapWidth + j] = Average(i, j);
		}
	}

	// Replace the old heightmap with the filtered one.
	m_Heightmap = dest;
}

bool Terrain::InBounds(int i, int j)
{
	// True if ij are valid indices; false otherwise.
	return
		i >= 0 && i < (int)m_HeightmapHeight &&
		j >= 0 && j < (int)m_HeightmapWidth;
}

float Terrain::Average(int i, int j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	// Use int to allow negatives.  If we use UINT, @ i=0, m=i-1=UINT_MAX
	// and no iterations of the outer for loop occur.
	for (int m = i - 1; m <= i + 1; ++m)
	{
		for (int n = j - 1; n <= j + 1; ++n)
		{
			if (InBounds(m, n))
			{
				avg += m_Heightmap[m * m_HeightmapWidth + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

void Terrain::CalcAllPatchBoundsY()
{
	m_PatchBoundsY.resize(m_NumPatchQuadFaces);

	// For each patch
	for (UINT i = 0; i < m_NumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < m_NumPatchVertCols - 1; ++j)
		{
			CalcPatchBoundsY(i, j);
		}
	}
}

void Terrain::CalcPatchBoundsY(UINT i, UINT j)
{
	// Scan the heightmap values this patch covers and compute the min/max height.
	UINT x0 = j*CellsPerPatch;
	UINT x1 = (j + 1)*CellsPerPatch;

	UINT y0 = i*CellsPerPatch;
	UINT y1 = (i + 1)*CellsPerPatch;

	float minY = +TERRAIN_INFINITY;
	float maxY = -TERRAIN_INFINITY;
	for (UINT y = y0; y <= y1; ++y)
	{
		for (UINT x = x0; x <= x1; ++x)
		{
			UINT k = y * m_HeightmapWidth + x;
			minY = Min(minY, m_Heightmap[k]);
			maxY = Max(maxY, m_Heightmap[k]);
		}
	}

	UINT patchID = i*(m_NumPatchVertCols - 1) + j;
	m_PatchBoundsY[patchID] = XMFLOAT2(minY, maxY);
}

const RenderInfo* Terrain::GetRenderInfo()
{
	m_RenderInfo.stride = sizeof(TerrainInfo);
	m_RenderInfo.offset = 0;
	m_RenderInfo.primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	return &m_RenderInfo;
}

const ConstantInfo* Terrain::GetConstantInfo()
{
	m_ConstantInfo.world = m_World;
	m_ConstantInfo.color = { 0, 0, 0, 0 };
	m_ConstantInfo.material.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_ConstantInfo.material.Diffuse = XMFLOAT4(0.4f, 0.6f, 0.6f, 1.0f);
	m_ConstantInfo.material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	m_ConstantInfo.material.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_ConstantInfo.heightmapHeight = m_HeightmapHeight;
	m_ConstantInfo.heightmapWidth = m_HeightmapWidth;
	m_ConstantInfo.cellSpacing = m_CellSpacing;

	return &m_ConstantInfo;
}

