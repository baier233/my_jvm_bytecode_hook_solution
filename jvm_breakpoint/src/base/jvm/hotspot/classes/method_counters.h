//
// Created by JKot on 2024/10/16.
//

#ifndef METHOD_COUNTERS_HPP
#define METHOD_COUNTERS_HPP
#include <cstdint>


class MethodCounters {
public:
	uint16_t number_of_breakpoints();

	void set_number_of_breakpoints(uint16_t number);
};


#endif // METHOD_COUNTERS_HPP
