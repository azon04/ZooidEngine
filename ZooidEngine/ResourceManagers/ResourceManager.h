#ifndef __ZE_RESOURCE_MANAGER__
#define __ZE_RESOURCE_MANAGER__

#include "Utils/PrimitiveTypes.h"
#include "Utils/HashMap.h"
#include "Memory/Handle.h"
#include "GameObjectModel/FunctionDelegates.h"
#include "GameObjectModel/Object.h"

namespace ZE
{
	// Internal class use by ResourceManager to keep track all the resources
	class Resource
	{
	public:
		Resource() : m_refCount(0) {}
		Resource(Handle handle)
			: m_hActual(handle), m_refCount(0)
		{}

		bool IsLoaded() { return m_hActual.isValid(); }

		Handle m_hActual;
		UInt32 m_refCount;
		Array<EventDelegate, true> m_loadedDelegates;
	};

	// struct that can pass to ResourceManager to let the ResourceManager now assets settings upon loading
	struct ResourceCreateSettings
	{

	};

	class ResourceManager : public Object
	{
		DEFINE_CLASS(ResourceManager)

	public:
		
		ResourceManager() {}
		ResourceManager(UInt32 maxResource) : m_maxResource(maxResource)
		{}

		// Load Resource Sync
		Handle loadResource(const char* resourceFilePath, ResourceCreateSettings* settings = nullptr);

		//Load Resource Sync and get the result
		template<class T>
		T* loadResource(const char* resourceFilePath, ResourceCreateSettings* settings = nullptr)
		{
			Handle h = loadResource(resourceFilePath, settings);
			return h.getObject<T>();
		}

		// Register resource to load ( not actually load the res, only register it )
		void loadResourceAsync(const char* resourceFilePath, EventDelegate callback = EventDelegate());

		// Register resource to unload ( not actually unload the res, only register it )
		void unloadResource(const char* resourceFilePath);

		// Load all resource that reference count more than zero (means that the resource still in used)
		void loadAllResource();

		// Unload all resource that reference count is zero ( means that nothing that used the assets )
		void unloadUnusedResource();

		// Unload all resource; No exceptions
		void unloadResources();

		// cycle that deal with load and unload
		void doLoadUnload();

		template<class T>
		T* getResource(const char* filePath)
		{
			return getResourceHandle(filePath).getObject<T>();
		}

		Handle getResourceHandle(const char* filePath);
		
	protected:
		// This must override in the child class to provide memory handle to actual resource pointer
		virtual Handle loadResource_Internal(const char* resourceFilePath, ResourceCreateSettings* options = nullptr) = 0;

		// Pre unload the resource. Do everything to clear the assets
		virtual void preUnloadResource(Resource* _resource) = 0;

	protected:
		HashMap<String, Resource> m_resourceMap;
		UInt32 m_maxResource;
	};

};

#endif
