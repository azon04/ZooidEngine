#include "ResourceManager.h"

#include "ZEngine.h"
#include "Events/Events.h"

namespace ZE
{

	IMPLEMENT_CLASS_0(ResourceManager)

	Handle ResourceManager::loadResource(const char* resourceFilePath)
	{
		if (m_resourceMap.hasKey(resourceFilePath))
		{
			Resource& resource = m_resourceMap[resourceFilePath];
			resource.m_refCount++;
			if (!resource.IsLoaded())
			{
				resource.m_hActual = loadResource_Internal(resourceFilePath);
			}
			return resource.m_hActual;
		}
		else
		{
			Resource resource;
			resource.m_refCount = 1;
			m_resourceMap.put(resourceFilePath, resource);
			resource.m_hActual = loadResource_Internal(resourceFilePath);
			return resource.m_hActual;
		}
	}

	void ResourceManager::loadResourceAsync(const char* resourceFilePath, EventDelegate callback)
	{
		if (m_resourceMap.hasKey(resourceFilePath))
		{
			m_resourceMap[resourceFilePath].m_refCount++;
			if (callback.isValid())
			{
				m_resourceMap[resourceFilePath].m_loadedDelegates.push_back(callback);

				if (m_resourceMap[resourceFilePath].IsLoaded())
				{
					Handle eventHandle("Event Loaded", sizeof(Event_RESOURCE_LOADED));
					Event_RESOURCE_LOADED* pEvent = new(eventHandle) Event_RESOURCE_LOADED();
					pEvent->m_resourceHandle = m_resourceMap[resourceFilePath].m_hActual;
					callback.call(pEvent);
					eventHandle.release();
				}
			}
		}
		else
		{
			Resource resource;
			resource.m_refCount = 1;
			if (callback.isValid())
			{
				resource.m_loadedDelegates.push_back(callback);
			}
			m_resourceMap.put(resourceFilePath, resource);
		}
	}

	void ResourceManager::unloadResource(const char* resourceFilePath)
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
		Array<String, true>& keys = m_resourceMap.getKeys();

		for (int k_index = 0; k_index < keys.length(); k_index++)
		{
			Resource& resource = m_resourceMap[keys[k_index].c_str()];
			if (resource.m_refCount > 0 && !resource.IsLoaded())
			{
				resource.m_hActual = loadResource_Internal(keys[k_index].c_str());

				Handle eventHandle("Event Loaded", sizeof(Event_RESOURCE_LOADED));
				Event_RESOURCE_LOADED* pEvent = new(eventHandle) Event_RESOURCE_LOADED();
				pEvent->m_resourceHandle = resource.m_hActual;
				int callBackSize = resource.m_loadedDelegates.length();
				for (int i = 0; i < callBackSize; ++i)
				{
					resource.m_loadedDelegates[i].call(pEvent);
				}
				eventHandle.release();
			}
		}
	}

	void ResourceManager::unloadUnusedResource()
	{
		Array<String, true> keys = m_resourceMap.getKeys();

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
