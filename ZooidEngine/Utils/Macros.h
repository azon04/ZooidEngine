#ifndef __ZE_MACROS_H__
#define __ZE_MACROS_H__

// This is macros helper

#if defined(WIN32) || defined(WIN64)
#define ZE_PLATFORM_WINDOWS
#else
#define ZE_PLATFORM_UNKNOWN
#endif

#define VISUAL_CPP 0
#define OTHER 999

#if _MSC_VER && !__INTEL_COMPILER // Using visual studio
	#define COMPILER	VISUAL_CPP
#else
	#define COMPILER	OTHER
#endif

// Platform specific support
#if defined(ZE_PLATFORM_WINDOWS)
#define ZE_SUPPORT_SIMD
#define USING_SSE 1
#else

#endif

#if defined(ZE_SUPPORT_SIMD)
#define ALIGN16 __declspec(align(16))
#else
#define ALIGN16
#endif

// Inline helper
#if COMPILER == VISUAL_CPP
	#define FORCEINLINE __forceinline
	#define INLINE __inline
#else
	#define FORCEINLINE inline
	#define INLINE inline
#endif

#define COMPARE_RETURN(InText, OutVar) \
	if( StringFunc::Compare(InText, #OutVar) == 0) \
	{ return OutVar; }

#endif
