#ifndef __ZE_SINGLETON_CLASS_H__
#define __ZE_SINGLETON_CLASS_H__

#include "Memory/Handle.h"

namespace ZE
{
	template<class T>
	class Singleton
	{
	public:		
		
		static T* GetInstance();

	private:

		static T* s_instance;
	};

	template<class T>
	T* ZE::Singleton<T>::s_instance = nullptr;

	template<class T>
	T* ZE::Singleton<T>::GetInstance()
	{
		if (!s_instance)
		{
			Handle h("Singleton", sizeof(T));
			s_instance = new(h) T;
		}

		return s_instance;
	}

};
#endif
