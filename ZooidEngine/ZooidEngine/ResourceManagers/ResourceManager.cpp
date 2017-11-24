#include "ResourceManager.h"

#include "ZEngine.h"

namespace ZE
{
	void ResourceManager::registerResourceToLoad(const char* resourceFilePath)
	{
		if (m_resourceMap.hasKey(resourceFilePath))
		{
			m_resourceMap[resourceFilePath].m_refCount++;
		}
		else
		{
			Resource resource;
			resource.m_refCount = 1;
			m_resourceMap.put(resourceFilePath, resource);
		}
	}

	void ResourceManager::registerResourceToUnLoad(const char* resourceFilePath)
	{
		if (m_resourceMap.hasKey(resourceFilePath))
		{
			m_resourceMap[resourceFilePath].m_refCount--;
		}
		else
		{
			ZEINFO("Resource to unload is not currently loaded: %s", resourceFilePath);
		}
	}

	void ResourceManager::loadAllResource()
	{
		Array<String, true> keys;
		keys = m_resourceMap.getKeys();

		for (int k_index = 0; k_index < keys.length(); k_index++)
		{
			Resource& resource = m_resourceMap[keys[k_index].c_str()];
			if (resource.m_refCount > 0 && !resource.IsLoaded())
			{
				resource.m_hActual = loadResource(keys[k_index].c_str());
			}
		}
	}

	void ResourceManager::unloadUnusedResource()
	{
		Array<String, true> keys;
		keys = m_resourceMap.getKeys();

		for (int k_index = 0; k_index < keys.length(); k_index++)
		{
			Resource& resource = m_resourceMap[keys[k_index].c_str()];
			if (resource.m_refCount <= 0 && resource.IsLoaded())
			{
				preUnloadResource(&resource);
				resource.m_hActual.release();
				m_resourceMap.erase(keys[k_index].c_str());
			}
		}
	}

	void ResourceManager::unloadResources()
	{
		Array<String, true> keys;
		keys = m_resourceMap.getKeys();

		for (int k_index = 0; k_index < keys.length(); k_index++)
		{
			Resource& resource = m_resourceMap[keys[k_index].c_str()];
			if (resource.IsLoaded())
			{
				preUnloadResource(&resource);
				resource.m_hActual.release();
			}
			m_resourceMap.erase(keys[k_index].c_str());
		}
	}

	void ResourceManager::doLoadUnload()
	{
		loadAllResource();
		unloadUnusedResource();
	}

	ZE::Handle ResourceManager::getResourceHandle(const char* filePath)
	{
		if (m_resourceMap.hasKey(filePath))
		{
			return m_resourceMap[filePath].m_hActual;
		}	
		return Handle();
	}

}
