#include "Thread.h"

ZE::ThreadId ZE::getThreadId()
{
	return std::this_thread::get_id();
}
