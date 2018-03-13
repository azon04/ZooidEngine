#ifndef __ZE_FILE_WRITER__
#define __ZE_FILE_WRITER__

#include <stdio.h>

#include "Utils/PrimitiveTypes.h"
#include "Utils/String.h"

namespace ZE
{
	class FileWriter
	{
	public:
		FileWriter() {}
		FileWriter(const char* filePath);

		void open(const char* filePath);
		void close();
		
		bool isValid();

		void writeFromBuffer(void* buffer, size_t bufferSize);
		void writeFormatText(const char* formatText, ...);

		char* getPath() { return &m_path[0]; }
		FILE* getInternalFileHandle() { return m_fileHandle; }

	protected:
		FILE* m_fileHandle;
		char m_path[256];
	};
};
#endif
