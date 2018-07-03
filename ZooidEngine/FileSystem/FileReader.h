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

		// Open file
		void open(const char* filePath);
		
		// Close file
		void close();

		// check whether the file reader is valid (i.e. file exist or not, or not readable)
		bool isValid() const;

		// Read file to buffer
		size_t readToBuffer(void* buffer, size_t bufferSize);
		
		// Reading for text string
		void readNextString(void* buffer);

		// Reading for int
		int readNextInt();

		// Reading for float
		float readNextFloat();

		// Check if file reaches end of the file
		bool eof() const;

		// Size of file, in byte
		long size();

		// Check current position of cursor
		long getCurrentPosition();

		// Get File Path
		String getPath() const { return m_path; }

		// Get actual File handle
		FILE* getInternalFileHandle() const { return m_fileHandle; }

	protected:
		FILE* m_fileHandle;
		String m_path;
	};
};
#endif
