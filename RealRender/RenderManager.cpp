#include "stdafx.h"
#include "RenderManager.h"
#include "RenderBase.h"

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
	Safe_Delete_VecList(m_vecRender);
}

void RenderManager::AddRender(RenderBase* pNewRenderBase)
{
	auto iter = STD_FIND_IF(m_vecRender, [&](RenderBase* pRenderBase)
	{
		return pRenderBase->GetRenderKey() == pNewRenderBase->GetRenderKey();
	});

	if (iter == m_vecRender.end())
	{
		m_vecRender.push_back(pNewRenderBase);
	}
}

void RenderManager::SetupConstant(RENDER_KEY renderKey, const ConstantInfo* info)
{
	auto iter = STD_FIND_IF(m_vecRender, [&](RenderBase* pRenderBase)
	{
		return pRenderBase->GetRenderKey() == renderKey;
	});
	if (iter != m_vecRender.end())
	{
		(*iter)->SetupConstant(info);
	}
	else
	{
		assert(false);
	}
}

void RenderManager::Render(RENDER_KEY renderKey, ObjectResource* pResource, const RenderInfo* info)
{
	auto iter = STD_FIND_IF(m_vecRender, [&](RenderBase* pRenderBase)
	{
		return pRenderBase->GetRenderKey() == renderKey;
	});
	if (iter != m_vecRender.end())
	{
		(*iter)->Render(pResource, info);
	}
	else
	{
		assert(false);
	}
}

void RenderManager::AddTextureSRV(RENDER_KEY renderKey, ID3D11ShaderResourceView* pTextureSRV)
{
	auto iter = STD_FIND_IF(m_vecRender, [&](RenderBase* pRenderBase)
	{
		return pRenderBase->GetRenderKey() == renderKey;
	});
	if (iter != m_vecRender.end())
	{
		(*iter)->AddTextureSRV(pTextureSRV);
	}
	else
	{
		assert(false);
	}
}
