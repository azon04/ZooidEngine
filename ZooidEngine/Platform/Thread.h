#ifndef __ZE_THREAD__
#define __ZE_THREAD__

// #TODO This is C++11, need to change if in another platform is not working
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace ZE 
{
	typedef std::thread Thread;
	typedef std::mutex Mutex;
	typedef std::condition_variable ConditionVariable;
	typedef std::thread::id ThreadId;
	typedef std::unique_lock<std::mutex> UniqueLock;
	
	ThreadId getThreadId();

	const ThreadId MAIN_THREAD_ID = ZE::getThreadId();
};

#endif

