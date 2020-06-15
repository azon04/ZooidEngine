#ifndef __ZE_UI_DATAPROVIDERS_H__
#define __ZE_UI_DATAPROVIDERS_H__

#include "ZooidUI.h"
#include "Utils/Array.h"
#include "Utils/String.h"

namespace ZE
{
	template<typename T>
	class ArrayListProvider : public ItemListProvider
	{
	public:
		virtual const UIChar* getItemAt(UInt32 index) const { return (const UIChar*) &m_interalArray.getConst(index); };
		virtual Int32 count() const { return m_interalArray.length(); }
		
		void setArray(const Array<T>& inArray) { m_interalArray = inArray; }
		Array<T>& getInternalArray() { return m_interalArray; }
	
	protected:
		Array<T> m_interalArray;

	};

	template<>
	class ArrayListProvider<String> : public ItemListProvider
	{
	public:
		virtual const UIChar* getItemAt(UInt32 index) const { return m_interalArray.getConst(index).c_str(); };
		virtual Int32 count() const { return m_interalArray.length(); }

		void setArray(const Array<String>& inArray) { m_interalArray = inArray; }
		Array<String>& getInternalArray() { return m_interalArray; }

	protected:
		Array<String> m_interalArray;

	};
	
}
#endif
