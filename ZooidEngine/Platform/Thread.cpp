#include "Thread.h"

ZE::ThreadId ZE::getThreadId()
{
	return std::this_thread::get_id();
}

void ZE::ThreadSleep(int millisecond)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
}
