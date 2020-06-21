#include "../Platform.h"

#include <windows.h>
#include <direct.h>
#include <stdlib.h>

#include "ZEGameContext.h"
#include "Input/InputManager.h"
#include "Input/KeyboardMouseInput.h"
#include "Logging/LogManager.h"
#include "Utils/ZEngineHelper.h"
#include "Utils/StringFunc.h"
#include "Utils/HashMap.h"
#include "Utils/Macros.h"

// Current OpenGL implementation using GLFW, so all input will be handled by GLFW
// Let's forward those inputs to the engine
#if ZE_RENDER_OPENGL
#include <GLFW/glfw3.h>

#include "Renderer/IRenderer.h"

ZE::HashMap<int, ZE::Key::KeyEnum> keyMap;
#define MAP_KEY(Key, EngineKey) keyMap.put(Key, EngineKey); 
#endif

DWORD ReadCPUSpeed();
void CheckCPU();
bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
bool CheckStorage(const DWORDLONG diskSpaceNeeded);

#if ZE_RENDER_OPENGL
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void charInputCallback(GLFWwindow* window, unsigned int codepoint);
void mousePositionCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void mouseButtonUpdateCallback(GLFWwindow* window, int button, int action, int mods);
void populateKeyMap();
#endif

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

		virtual void postInit()
		{
#if ZE_RENDER_OPENGL
			IRenderer* renderer = gGameContext->getRenderer();

			glfwSetCursorPosCallback((GLFWwindow*)renderer->getWindowContext(), mousePositionCallback);
			glfwSetMouseButtonCallback((GLFWwindow*)renderer->getWindowContext(), mouseButtonUpdateCallback);
			glfwSetKeyCallback((GLFWwindow*)renderer->getWindowContext(), keyCallback);
			glfwSetCharCallback((GLFWwindow*)renderer->getWindowContext(), charInputCallback);
			glfwSetScrollCallback((GLFWwindow*)renderer->getWindowContext(), scrollCallback);

			populateKeyMap();
#endif
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

			StringFunc::WriteTo(buff, m_args.paramArgs[index], StringFunc::Length(m_args.paramArgs[index])+1);
			
			return true;
		}

		virtual int getArgCount()
		{
			return m_args.paramCount;
		}

		virtual Float32 randUnitFloat()
		{
			return rand() / float(RAND_MAX);
		}

	public:
		// Handle Input functions
		void handleKey(Short key, KeyState::KeyStateEnum keyState)
		{
			KeyboardMouseInput* keyboardMouseInput = gGameContext->getInputManager()->getKeyboardMouseInput();

			keyboardMouseInput->setKey(key, keyState);
		}

		void handleCharacterInput(unsigned int charCode)
		{

		}

		void handleMousePosition(int xPos, int yPos)
		{
			KeyboardMouseInput* keyboardMouseInput = gGameContext->getInputManager()->getKeyboardMouseInput();

			keyboardMouseInput->setMousePosition(xPos, yPos);
		}

		void handleMouseScroll(double xOffset, double yOffset)
		{

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

#if ZE_RENDER_OPENGL
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	ZE::Win_Platform* plat = (ZE::Win_Platform*) ZE::Platform::GetPlatform();

	ZE::KeyState::KeyStateEnum state = ZE::KeyState::Pressed;

	switch (action)
	{
	case GLFW_PRESS:
		state = ZE::KeyState::Pressed;
		break;
	case GLFW_RELEASE:
		state = ZE::KeyState::Released;
		break;
	case GLFW_REPEAT:
		state = ZE::KeyState::Repeat;
		break;
	}

	if (key > 255)
	{
		ZE::Short engineKey = keyMap[key];
		plat->handleKey(engineKey, state);
	}
	else
	{
		plat->handleKey(key, state);
	}
}

void charInputCallback(GLFWwindow* window, unsigned int codepoint)
{

}

void mousePositionCallback(GLFWwindow* window, double xPos, double yPos)
{
	ZE::Win_Platform* plat = (ZE::Win_Platform*) ZE::Platform::GetPlatform();

	plat->handleMousePosition(xPos, yPos);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{

}

void mouseButtonUpdateCallback(GLFWwindow* window, int button, int action, int mods)
{
	ZE::KeyState::KeyStateEnum state = ZE::KeyState::Pressed;

	switch (action)
	{
	case GLFW_PRESS:
		state = ZE::KeyState::Pressed;
		break;
	case GLFW_RELEASE:
		state = ZE::KeyState::Released;
		break;
	case GLFW_REPEAT:
		state = ZE::KeyState::Repeat;
		break;
	}

	ZE::Key::KeyEnum mouseButton = ZE::Key::None;

	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		mouseButton = ZE::Key::MouseLeftButton;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mouseButton = ZE::Key::MouseRightButton;
		break;
	}

	if (mouseButton != ZE::Key::None)
	{
		ZE::Win_Platform* plat = (ZE::Win_Platform*) ZE::Platform::GetPlatform();

		plat->handleKey(mouseButton, state);
	}
}

void populateKeyMap()
{
	MAP_KEY(GLFW_KEY_ESCAPE, ZE::Key::Esc)
	MAP_KEY(GLFW_KEY_UP, ZE::Key::UpArrow)
	MAP_KEY(GLFW_KEY_DOWN, ZE::Key::DownArrow)
	MAP_KEY(GLFW_KEY_LEFT, ZE::Key::LeftArrow)
	MAP_KEY(GLFW_KEY_RIGHT, ZE::Key::RightArrow)
}
#endif