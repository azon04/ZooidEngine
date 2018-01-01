#include "FileWriter.h"

#include "ZEngine.h"

namespace ZE {
	
	FileWriter::FileWriter(const char* filePath)
	{
		open(filePath);
	}

	void FileWriter::open(const char* filePath)
	{
		m_path = filePath;
		m_fileHandle = fopen(filePath, "w");

		if (m_fileHandle == nullptr)
		{
			ZEINFO("Can't open file to write : %s ", filePath);
		}

	}

	void FileWriter::close()
	{
		if (m_fileHandle)
		{
			fclose(m_fileHandle);
			m_fileHandle = nullptr;
		}
	}

	bool FileWriter::isValid()
	{
		return m_fileHandle != nullptr;
	}

	void FileWriter::writeFromBuffer(void* buffer, size_t bufferSize)
	{
		fwrite(buffer, bufferSize, 1, m_fileHandle);
	}

	

}
