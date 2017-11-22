#ifndef __ZE_RESOURCE_MANAGER__
#define __ZE_RESOURCE_MANAGER__

#include "Utils/PrimitiveTypes.h"
#include "Utils/HashMap.h"
#include "MemoryManagement/Handle.h"

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
	};

	class ResourceManager
	{
	public:
		
		ResourceManager() {}
		ResourceManager(UInt32 maxResource) : m_maxResource(maxResource)
		{}

		// Register resource to load ( not actually load the res, only register it )
		void registerResourceToLoad(const char* resourceFilePath);

		// Register resource to unload ( not actually unload the res, only register it )
		void registerResourceToUnLoad(const char* resourceFilePath);

		// Load all resource that reference count more than zero (means that the resource still in used)
		void loadAllResource();

		// Unload all resource that reference count is zero ( means that nothing that used the assets )
		void unloadUnusedResource();

		// cycle that deal with load and unload
		void doLoadUnload();

		// This must override in the child class to provide memory handle to actual resource pointer
		virtual Handle loadResource(const char* resourceFilePath) = 0;

	protected:
		HashMap<String, Resource> m_resourceMap;
		UInt32 m_maxResource;
	};

};

#endif
