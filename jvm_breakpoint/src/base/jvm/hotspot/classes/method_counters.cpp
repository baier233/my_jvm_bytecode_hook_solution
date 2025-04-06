#include "method_counters.h"
#include <base/jvm/hotspot/utility/jvm_internal.h>
uint16_t MethodCounters::number_of_breakpoints() {
	const auto _numbers_of_breakpoints = VMWrapper::as("MethodCounters", this);
	if (_numbers_of_breakpoints == nullptr) {
		throw std::invalid_argument("MethodCounters not found");
	}
	return _numbers_of_breakpoints->get_field<uint16_t>("_number_of_breakpoints");
}

void MethodCounters::set_number_of_breakpoints(uint16_t number) {
	const auto _numbers_of_breakpoints = VMWrapper::as("MethodCounters", this);
	if (_numbers_of_breakpoints == nullptr) {
		throw std::invalid_argument("MethodCounters not found");
	}
	_numbers_of_breakpoints->set_field<uint16_t>("_number_of_breakpoints", number);
}
