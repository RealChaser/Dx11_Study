#pragma once
#include "Singleton.h"
#include "ShareValue.h"

class ResourceManager : public Singleton<ResourceManager>
{
	friend class Singleton<ResourceManager>;
private:
	ResourceManager() = default;
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

public:
	~ResourceManager();

public:
	inline void Add(string objectName, ObjectResource* objectResource){ Add({ objectName, objectResource }); }
	inline void Add(pair<string, ObjectResource*> pairValue){ m_mapResources.insert(pairValue); }

	ObjectResource* GetObjectResource(string objectName){ return m_mapResources[objectName]; }
	bool CheckExist(string objectName);
	
	
private:
	map<string, ObjectResource*> m_mapResources;
};

