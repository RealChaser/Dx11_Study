#include "stdafx.h"
#include "ResourceManager.h"
#include "TemplateUtil.h"

ResourceManager::~ResourceManager()
{
	Safe_Delete_Map(m_mapResources);
}

bool ResourceManager::CheckExist(string objectName)
{
	auto iter = m_mapResources.find(objectName);
	if (iter == m_mapResources.end())
		return false;

	return true;
}
