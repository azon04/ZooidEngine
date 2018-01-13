#include "FileReader.h"

#include "ZEngine.h"

namespace ZE
{
	FileReader::FileReader(const char* filePath)
	{
		open(filePath);
	}

	void FileReader::open(const char* filePath)
	{
		m_path = filePath;
		m_fileHandle = fopen(filePath, "r");

		if (m_fileHandle == nullptr)
		{
			ZEINFO("Can't find file : %s ", filePath);
		}
	}

	void FileReader::close()
	{
		if (m_fileHandle)
		{
			fclose(m_fileHandle);
			m_fileHandle = nullptr;
		}
	}

	bool FileReader::isValid()
	{
		return m_fileHandle != nullptr;
	}

	size_t FileReader::readToBuffer(void* buffer, size_t bufferSize)
	{
		return fread_s(buffer, bufferSize, 1, bufferSize , m_fileHandle);
	}

	void FileReader::readNextString(void* buffer)
	{
		fscanf(m_fileHandle, "%s", buffer);
	}

	int FileReader::readNextInt()
	{
		int result;
		fscanf(m_fileHandle, "%d", &result);
		return result;
	}

	float FileReader::readNextFloat()
	{
		float result;
		fscanf(m_fileHandle, "%f", &result);
		return result;
	}

	bool FileReader::eof()
	{
		return feof(m_fileHandle) > 0;
	}

	long FileReader::size()
	{
		long lastPos = ftell(m_fileHandle);
		fseek(m_fileHandle, 0L, SEEK_END);
		long size = ftell(m_fileHandle);
		fseek(m_fileHandle, lastPos, SEEK_SET);
		
		return size;
	}

	long FileReader::getCurrentPosition()
	{
		return ftell(m_fileHandle);
	}

}
