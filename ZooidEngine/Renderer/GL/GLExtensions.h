#ifndef __ZE_GL_EXTENSIONS__
#define __ZE_GL_EXTENSIONS__

// GL Extensions : Handle all extension needed for the project

#include "Platform/Platform.h"

#if ZE_PLATFORM_WINDOWS
#include "GL/wglext.h"
#endif

namespace ZE
{
	// #TODO Define all typedef for function

	// #TODO Extension helper function

#if ZE_PLATFORM_WINDOWS
	void *GetGLFuncAddress(const char* name)
	{
		void *p = (void*)wglGetProcAddress(name);
		if (p == 0 ||
			(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
			(p == (void*)-1))
		{
			HMODULE module = LoadLibraryA("opengl32.dll");
			p = (void *)GetProcAddress(module, name);
			return p;
		}
		else
		{
			return nullptr;
		}
	}
#else
#error "Neede GL Extension support for this platform"
#endif
}
#endif
