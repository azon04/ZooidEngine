#include "../Platform.h"

#include <windows.h>
#include <direct.h>

#include "Logging/LogManager.h"
#include "Utils/ZEngineHelper.h"
#include "Utils/StringFunc.h"

DWORD ReadCPUSpeed();
void CheckCPU();
bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
bool CheckStorage(const DWORDLONG diskSpaceNeeded);

namespace ZE
{
	class Win_Platform : public Platform
	{
	protected:
		PlatformArgs m_args;
		int m_erroCode;
		bool m_bRequestExit;

	public:
		Win_Platform() : m_bRequestExit(false) {}

	protected:
		virtual void init(PlatformArgs& args)
		{
			m_args = args;

			int index = 0;
			int count = 0;
			while (m_args.rawCommandLine[index++] != '\0')
			{
				if (index == 1)
				{
					m_args.paramArgs[count++] = m_args.rawCommandLine + (index - 1);
				}
				else if (m_args.rawCommandLine[index - 1] == ' ')
				{
					m_args.rawCommandLine[index - 1] = '\0';
					if (m_args.rawCommandLine[index] != '\0')
					{
						m_args.paramArgs[count++] = m_args.rawCommandLine + index;
					}
				}

			}

			m_args.paramCount = count;

			for (int i = 0; i < m_args.paramCount; i++)
			{
				// Activate Console
				if (StringFunc::Compare(m_args.paramArgs[i], "--log") == 0)
				{
					AllocConsole();
					AttachConsole(GetCurrentProcessId());
					freopen("CON", "w", stdout);
				}
			}

			CheckCPU();
			CheckMemory(0, 0);
			CheckStorage(0);
		}

		virtual void requestExit(int errorCode)
		{
			m_bRequestExit = true;
			m_erroCode = errorCode;
		}

		virtual bool isRequestedExit()
		{
			return m_bRequestExit;
		}

		virtual void destroy()
		{

		}

		virtual int getExitErrorCode()
		{
			return m_erroCode;
		}

		virtual PlatformArgs* getArgs()
		{
			return &m_args;
		}

		virtual bool getArgByIndex(int index, char* buff)
		{
			if (index >= m_args.paramCount)
			{
				return false;
			}

			StringFunc::WriteTo(buff, m_args.paramArgs[index], StringFunc::Length(m_args.paramArgs[index]));
			
			return true;
		}

		virtual int getArgCount()
		{
			return m_args.paramCount;
		}
	};

	Platform* Platform::GetPlatform()
	{
		static Win_Platform winPlatform;
		return &winPlatform;
	}
}

DWORD ReadCPUSpeed()
{
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;

	long lError = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		RegQueryValueExA(hKey, "~MHz", NULL, &type, (LPBYTE)&dwMHz, &BufSize);
	}

	return dwMHz;
}

void CheckCPU()
{
	DWORD BufSize = sizeof(char) * 256;
	char dwCPUName[256];
	DWORD type = REG_SZ;
	HKEY hKey;

	long lError = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		RegQueryValueExA(hKey, "ProcessorNameString", NULL, &type, (LPBYTE)&dwCPUName, &BufSize);
	}

	ZELOG(ZE::LOG_ENGINE, ZE::Log, "CPU Processor: %s", dwCPUName);
	ZELOG(ZE::LOG_ENGINE, ZE::Log, "CPU Speed: %d MHz", ReadCPUSpeed());

}
#define DIV_GB ((1 << 30) * 1.0f)

bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded)
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);

	ZELOG(ZE::LOG_ENGINE, ZE::Log, "RAM Detected");
	ZELOG(ZE::LOG_ENGINE, ZE::Log, "Total Physical RAM: %f GB", (status.ullTotalPhys / DIV_GB));
	ZELOG(ZE::LOG_ENGINE, ZE::Log, "Available Physical RAM: %f GB", (status.ullAvailPhys / DIV_GB));

	if (physicalRAMNeeded > status.ullTotalPhys)
	{
		return false;
	}

	ZELOG(ZE::LOG_ENGINE, ZE::Log, "Total Virtual Memory: %f GB", (status.ullTotalVirtual / DIV_GB));
	ZELOG(ZE::LOG_ENGINE, ZE::Log, "Avail Virtual Memory: %f GB", (status.ullAvailVirtual / DIV_GB));

	if (virtualRAMNeeded > status.ullAvailVirtual)
	{
		return false;
	}

	// Make sure we have contiguous memory before hand
	if (virtualRAMNeeded > 0)
	{
		char *buff = new char[virtualRAMNeeded];
		if (buff)
		{
			delete[] buff;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool CheckStorage(const DWORDLONG diskSpaceNeeded)
{
	// Check for enough free disk space on the current disk
	int const drive = _getdrive();
	struct _diskfree_t diskFree;

	_getdiskfree(drive, &diskFree);

	unsigned __int64 const totalAvailDisk = diskFree.avail_clusters * diskFree.sectors_per_cluster * diskFree.bytes_per_sector;
	ZELOG(ZE::LOG_ENGINE, ZE::Log, "Total Available Disk in %c: %f GB", (char)('A' + drive - 1), (totalAvailDisk / DIV_GB));

	return diskSpaceNeeded < totalAvailDisk;
}