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

		// Open File
		void open(const char* filePath);

		// Close file
		void close();
		
		// Check if file writer is valid (i.e can create file, or file is writable)
		bool isValid();

		// Write to file from given buffer and size
		void writeFromBuffer(void* buffer, size_t bufferSize);

		// Write formatted text to file
		void writeFormatText(const char* formatText, ...);

		char* getPath() { return &m_path[0]; }
		FILE* getInternalFileHandle() { return m_fileHandle; }

	protected:
		FILE* m_fileHandle;
		char m_path[256];
	};
};
#endif
