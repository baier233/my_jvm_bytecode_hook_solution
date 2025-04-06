#pragma once
#include <hotspot/classes/java_thread.h>
#include <hotspot/classes/c_method.h>
class MethodCounters;

namespace vm_function {
	using BuildMethodCountersType = MethodCounters * (*) (java_hotspot::thread_base* current, java_hotspot::method* method);

	inline BuildMethodCountersType build_method_counters = nullptr;
} // namespace vm_function

