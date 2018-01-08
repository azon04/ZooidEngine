#ifndef __ZE_FILE_READER__
#define __ZE_FILE_READER__

#include <stdio.h>

#include "Utils/PrimitiveTypes.h"
#include "Utils/String.h"

namespace ZE
{
	class FileReader
	{
	public:
		FileReader()
			: m_fileHandle(nullptr) {}
		
		FileReader(const char* filePath);

		void open(const char* filePath);
		void close();

		bool isValid();

		size_t readToBuffer(void* buffer, size_t bufferSize);
		
		// Reading for text string
		void readNextString(void* buffer);

		// Reading for int
		int readNextInt();

		// Reading for float
		float readNextFloat();

		long size();
		long getCurrentPosition();

		String getPath() { return m_path; }
		FILE* getInternalFileHandle() { return m_fileHandle; }

	protected:
		FILE* m_fileHandle;
		String m_path;
	};
};
#endif
