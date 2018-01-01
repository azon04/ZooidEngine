#ifndef __ZE_MACROS_H__
#define __ZE_MACROS_H__

// This is macros helper

#define VISUAL_CPP 0
#define OTHER 999

#if _MSC_VER && !__INTEL_COMPILER // Using visual studio
	#define COMPILER	VISUAL_CPP
#else
	#define COMPILER	OTHER
#endif


// Inline helper
#if COMPILER == VISUAL_CPP
	#define FORCEINLINE __forceinline
	#define INLINE __inline
#else
	#define FORCEINLINE inline
	#define INLINE inline
#endif

#endif
