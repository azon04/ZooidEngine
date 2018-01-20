#ifndef __ZE_TIMER_H__
#define __ZE_TIMER_H__

#include "Utils/PrimitiveTypes.h"
#include "Utils/Macros.h"

#if __cplusplus <= 199711L
	#include <chrono>
	
	typedef std::chrono::high_resolution_clock::time_point Time;
	typedef std::chrono::duration<double, std::milli> DeltaTime;

	#define CPP11_CHRONO 1
#else 
	#include <time.h>
	
	typedef timespec Time;
	typedef double DeltaTime;

#endif

namespace ZE
{
	class Timer
	{
	public:
		Timer() 
		{
			t = Now();
		}

		virtual ~Timer() {}

		void Reset() 
		{
			Time now = Now();
#if CPP11_CHRONO
			delta = now - t;
#else
			delta = Delta(now, t);
#endif
			t = now;
		}
		
		FORCEINLINE double GetDeltaMS() 
		{ 
#if CPP11_CHRONO
			return delta.count();
#else
			return delta; 
#endif
		}

		FORCEINLINE double ResetAndGetDeltaMS() 
		{ 
			Reset();
			return GetDeltaMS(); 
		}

		// Inner timer representation
		Time t;
		DeltaTime delta;

		static FORCEINLINE Time Now() {
#if CPP11_CHRONO
			return std::chrono::high_resolution_clock::now();
#else
			Time ret;
			timespec_get(&ret, TIME_UTC);
			return ret;
#endif
		}
		
#if !CPP11_CHRONO
		static FORCEINLINE DeltaTime Delta(const Time& time1, const Time& time2)
		{
			return (time1.tv_sec - time2.tv_sec) * 1000.0 + (time1.tv_nsec - time2.tv_nsec) / 1000000.0;
		}
#endif

	};

}
#endif
