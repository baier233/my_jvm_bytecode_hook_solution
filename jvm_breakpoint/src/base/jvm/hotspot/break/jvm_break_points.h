﻿//
// Created by Administrator on 2024/3/17.
//

#ifndef JVM_BREAK_POINTS_H
#define JVM_BREAK_POINTS_H

#include <functional>
#include <map>
#include <vector>


#include "../include_header.h"
#include <titan_hook.h>
#include "break_point_info.h"
#include  <mutex>

namespace jvm_break_points {
	inline TitanHook<breakpoint_callback_t> breakpoint_hook;
	inline TitanHook<original_bytecode_handler_t> original_bytecode_hook;
	inline std::mutex o_bytecode_mutex;
	inline std::map<uintptr_t, uint8_t> original_bytecodes;
	inline std::mutex callback_mutex;
	inline std::map<uintptr_t, native_callback_t> breakpoint_callbacks;

	auto breakpoint_callback(
		java_hotspot::java_thread* java_thread,
		java_hotspot::method* method,
		uintptr_t bytecode_address
	) -> void;

	auto breakpoint_handler(
		java_hotspot::java_thread* java_thread,
		java_hotspot::method* method,
		uintptr_t bytecode_address,
		uintptr_t parameters,
		uintptr_t result_val_addr
	) -> void;

	auto original_bytecode_handler(
		java_hotspot::java_thread* java_thread,
		java_hotspot::method* method,
		uintptr_t bytecode_address
	) -> uint8_t;

	auto set_breakpoint(
		java_hotspot::method* method,
		uintptr_t offset,
		const native_callback_t& callback
	) -> bool;

	auto set_breakpoint_with_original_code(
		java_hotspot::method* method,
		uintptr_t offset,
		uint8_t original,
		const native_callback_t& callback
	) -> bool;

	auto remove_breakpoint(
		java_hotspot::method* method,
		uintptr_t offset
	) -> void;

	auto remove_all_breakpoints(
		java_hotspot::method* method
	) -> void;

	auto remove_all_breakpoints_cleaned(
		java_hotspot::method* method
	) -> std::vector<std::pair<uint8_t*, uint8_t>>;
}


#endif //JVM_BREAK_POINTS_H
