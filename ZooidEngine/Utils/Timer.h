#ifndef __ZE_TIMER_H__
#define __ZE_TIMER_H__

#include "Utils/PrimitiveTypes.h"
#include "Utils/Macros.h"

#if __cplusplus <= 199711L
	#include <chrono>
	
	typedef std::chrono::high_resolution_clock::time_point Time;
	typedef std::chrono::duration<double, std::milli> DeltaTime;
	typedef std::chrono::duration<double> DeltaSeconds;

	#define CPP11_CHRONO 1
#else 
	#include <time.h>
	
	typedef timespec Time;
	typedef double DeltaTime;
	typedef double DeltaSeconds;

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
			Time now = Now();

#if CPP11_CHRONO
			delta = now - t;
			return delta.count();
#else
			delta = Delta(now, t);
			return delta; 
#endif
		}

		FORCEINLINE double GetDeltaSeconds()
		{
			Time now = Now();
#if CPP11_CHRONO
			DeltaSeconds deltaSeconds = now - t;
			return deltaSeconds.count();
#else
			return CalcDeltaSeconds(now, t);
#endif
		}

		FORCEINLINE double ResetAndGetDeltaMS() 
		{ 
			Reset();
#if CPP11_CHRONO
			return delta.count();
#else
			return delta;
#endif 
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

		static FORCEINLINE DeltaSeconds CalcDeltaSeconds(const Time& time1, const Time& time2)
		{
			return (time1.tv_sec - time2.tv_sec) + (time1.tv_nsec - time2.tv_nsec) / 1000.0;
		}
#endif

	};

}
#endif
