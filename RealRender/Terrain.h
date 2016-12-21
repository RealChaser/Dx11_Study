#pragma once
#include "Object.h"
class Terrain :	public Object
{	
public:
	Terrain();
	virtual ~Terrain();

	ALIGNED16(Terrain);
public:
	virtual void Init();
	virtual void Update(float dt);
	virtual void Render();
	virtual void Cleanup();

protected:
	virtual void InitResource(ObjectResource* resource);
	virtual void InitBuffer(ObjectResource* resource);	
	virtual void InitTexture(ObjectResource* resource);		

public:
	virtual const RenderInfo* GetRenderInfo();
	virtual const ConstantInfo* GetConstantInfo();
	virtual void SetupConstant();

public:
	inline float GetWidth()const	{ return (m_HeightmapWidth - 1) * m_CellSpacing; }
	inline float GetDepth()const	{ return (m_HeightmapHeight - 1) * m_CellSpacing; }
	float GetHeight(float x, float z)const;

private:
	void LoadHeightmap();
	void Smooth();
	bool InBounds(int i, int j);
	float Average(int i, int j);
	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT i, UINT j);		

private:
	// Divide heightmap into patches such that each patch has CellsPerPatch cells
	// and CellsPerPatch+1 vertices.  Use 64 so that if we tessellate all the way 
	// to 64, we use all the data from the heightmap.  
	static const int CellsPerPatch = 64;			

	UINT m_NumPatchVertices;
	UINT m_NumPatchQuadFaces;

	UINT m_NumPatchVertRows;
	UINT m_NumPatchVertCols;	

	Material m_Mat;

	wstring m_HeightMapFilename;
	wstring m_LayerMapFilename0;
	wstring m_LayerMapFilename1;
	wstring m_LayerMapFilename2;
	wstring m_LayerMapFilename3;
	wstring m_LayerMapFilename4;
	wstring m_BlendMapFilename;

	UINT m_HeightmapWidth;
	UINT m_HeightmapHeight;
	float m_CellSpacing;
	float m_HeightScale;

	std::vector<XMFLOAT2> m_PatchBoundsY;
	std::vector<float> m_Heightmap;	
};

