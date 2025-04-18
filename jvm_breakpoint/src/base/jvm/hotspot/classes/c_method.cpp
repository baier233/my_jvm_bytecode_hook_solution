﻿//
// Created by Administrator on 2024/3/15.
//

#include "c_method.h"

#include "symbol.h"
#include "bytecode.h"
#include "../break/jvm_break_points.h"

auto java_hotspot::const_method::get_constants() -> const_pool* {
	static VMStructEntry* _constants_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_constants"];
	if (!_constants_entry) return nullptr;
	return *reinterpret_cast<const_pool**>(reinterpret_cast<uint8_t*>(this) + _constants_entry->offset);
}

auto java_hotspot::const_method::set_constants(const_pool* cp) -> void
{
	static VMStructEntry* _constants_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_constants"];
	if (!_constants_entry) return;
	*reinterpret_cast<const_pool**>(reinterpret_cast<uint8_t*>(this) + _constants_entry->offset) = cp;
}

unsigned short java_hotspot::const_method::get_code_size() {
	static VMStructEntry* _code_size_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_code_size"];
	if (!_code_size_entry) return 0;
	return *reinterpret_cast<unsigned short*>(reinterpret_cast<uint8_t*>(this) + _code_size_entry->offset);
}

unsigned short java_hotspot::const_method::get_name_index() {
	static VMStructEntry* _name_index_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_name_index"];
	if (!_name_index_entry) return 0;
	return *reinterpret_cast<unsigned short*>(reinterpret_cast<uint8_t*>(this) + _name_index_entry->offset);
}

unsigned short java_hotspot::const_method::get_signature_index() {
	static VMStructEntry* _signature_index_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()[
		"_signature_index"];
	if (!_signature_index_entry) return 0;
	return *reinterpret_cast<unsigned short*>(reinterpret_cast<uint8_t*>(this) + _signature_index_entry->offset);
}

auto java_hotspot::const_method::get_const_method_size() -> unsigned short {
	static VMStructEntry* _const_method_size_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()[
		"_const_method_size"];
	if (!_const_method_size_entry) return 0;
	return *reinterpret_cast<unsigned short*>(reinterpret_cast<uint8_t*>(this) + _const_method_size_entry->offset);
}

auto java_hotspot::const_method::get_max_stack() -> unsigned short {
	static VMStructEntry* _max_stack_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_max_stack"];
	if (!_max_stack_entry) return 0;
	return *reinterpret_cast<unsigned short*>(reinterpret_cast<uint8_t*>(this) + _max_stack_entry->offset);
}

auto java_hotspot::const_method::set_max_stack(const unsigned short max_stack) -> void {
	static VMStructEntry* _max_stack_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_max_stack"];
	if (!_max_stack_entry) return;
	*reinterpret_cast<unsigned short*>(reinterpret_cast<uint8_t*>(this) + _max_stack_entry->offset) = max_stack;
}

auto java_hotspot::const_method::get_max_locals() -> unsigned short {
	static VMStructEntry* _max_locals_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_max_locals"];
	if (!_max_locals_entry) return 0;
	return *reinterpret_cast<unsigned short*>(reinterpret_cast<uint8_t*>(this) + _max_locals_entry->offset);
}

auto java_hotspot::const_method::set_max_locals(const unsigned short max_locals) -> void {
	static VMStructEntry* _max_locals_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_max_locals"];
	if (!_max_locals_entry) return;
	*reinterpret_cast<unsigned short*>(reinterpret_cast<uint8_t*>(this) + _max_locals_entry->offset) = max_locals;
}

auto java_hotspot::const_method::get_size_of_parameters() -> unsigned short {
	static VMStructEntry* _size_of_parameters_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()[
		"_size_of_parameters"];
	if (!_size_of_parameters_entry) return 0;
	return *reinterpret_cast<unsigned short*>(reinterpret_cast<uint8_t*>(this) + _size_of_parameters_entry->offset);
}

auto java_hotspot::const_method::get_method_idnum() -> unsigned short {
	static VMStructEntry* _method_idnum_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()[
		"_method_idnum"];
	if (!_method_idnum_entry) return 0;
	return *reinterpret_cast<unsigned short*>(reinterpret_cast<uint8_t*>(this) + _method_idnum_entry->offset);
}

auto java_hotspot::const_method::get_bytecode_start() -> uint8_t* {
	return reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(this) + bytecode_start_offset);
}

auto java_hotspot::const_method::get_bytecode_size() -> int {
	/* This is a more efficient way to get the bytecode size */
	return get_code_size();
	/*uint8_t *start = get_bytecode_start();
	auto bytecode = std::make_unique<java_runtime::bytecode>(start);
	while (bytecode->get_opcode() != java_runtime::bytecodes::invalid) {
		const int length = bytecode->get_length();
		start += length;
		bytecode = std::make_unique<java_runtime::bytecode>(start);
	}
	return static_cast<int>(start - get_bytecode_start());*/
}

auto java_hotspot::const_method::set_bytecode(const std::vector<uint8_t>& bytecode) -> void {
	const int bytecode_size = get_bytecode_size();
	if (bytecode_size < bytecode.size()) {
		std::cerr << "New bytecode is larger than the original" << std::endl;
		return;
	}
	memcpy(get_bytecode_start(), bytecode.data(), bytecode.size());
	/* if the new bytecode is smaller than the original, fill the rest with nops */
	memset(get_bytecode_start() + bytecode.size(), static_cast<uint8_t>(java_runtime::bytecodes::_nop),
		bytecode_size - bytecode.size());
}

auto java_hotspot::const_method::get_bytecode() -> std::vector<uint8_t> {
	const int bytecode_size = get_bytecode_size();
	std::vector<uint8_t> bytecode(bytecode_size);
	memcpy(bytecode.data(), get_bytecode_start(), bytecode_size);
	return bytecode;
}
#ifdef JDK22FEATURE
auto java_hotspot::const_method::get_const_flags_jdk22() -> ConstMethodFlags*
{

	if (!this) return nullptr;
	static VMStructEntry* vm_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_flags"];
	if (!vm_entry)
		return nullptr;
	return (ConstMethodFlags*)((uint8_t*)this + vm_entry->offset);
}
#endif // JDK22FEATURE
auto java_hotspot::const_method::get_const_flags() -> u2
{
	if (!this) return 0;
	static VMStructEntry* vm_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_flags"];
	if (!vm_entry)
		return 0;
	return *(u2*)((uint8_t*)this + vm_entry->offset);
}

auto java_hotspot::const_method::get_localvariable_table_length_addr() -> void*
{
	MY_ASSERT(has_localvariable_table(), "called only if table is present");

	if (has_exception_handler()) {
		// If exception_table present, locate immediately before them.
		return (u2*)exception_table_start() - 1;
	}
	else {
		if (has_checked_exceptions()) {
			// If checked_exception present, locate immediately before them.
			return (u2*)checked_exceptions_start() - 1;
		}
		else {
			if (has_method_parameters()) {
				// If method parameters present, locate immediately before them.
				return (u2*)method_parameters_start() - 1;
			}
			else {
				// Else, the exception table is at the end of the constMethod.
				return has_generic_signature() ? (get_last_u2_element() - 1) :
					get_last_u2_element();
			}
		}
	}
	return nullptr;
}

auto java_hotspot::const_method::get_exception_table_length_addr() -> void*
{
	MY_ASSERT(has_exception_handler(), "called only if table is present");
	if (has_checked_exceptions()) {
		// If checked_exception present, locate immediately before them.
		return (unsigned short*)checked_exceptions_start() - 1;
	}
	else {
		if (has_method_parameters()) {
			// If method parameters present, locate immediately before them.
			return (unsigned short*)method_parameters_start() - 1;
		}
		else {
			// Else, the exception table is at the end of the constMethod.
			return has_generic_signature() ? (get_last_u2_element() - 1) :
				get_last_u2_element();
		}
	}
}

auto java_hotspot::const_method::get_last_u2_element() -> u2*
{
	int offset = 0;
	if (has_method_annotations()) offset++;
	if (has_parameter_annotations()) offset++;
	if (has_type_annotations()) offset++;
	if (has_default_annotations()) offset++;
	auto end = ((intptr_t*)this + get_size());
	return (u2*)((void**)end - offset) - 1;
}

auto java_hotspot::const_method::get_const_method_length() -> size_t {
	static VMTypeEntry* _constMethod_entry = JVMWrappers::find_type("ConstMethod").value();
	if (!_constMethod_entry)return 0;
	return _constMethod_entry->size;
}

auto java_hotspot::const_method::get_size() -> int
{
	//_constMethod_size
	static VMStructEntry* _constMethod_size_entry = JVMWrappers::find_type_fields("ConstMethod").value().get()["_constMethod_size"];
	return *(int*)(this + _constMethod_size_entry->offset);
}

std::vector<java_hotspot::local_variable_entry> java_hotspot::const_method::get_local_variable_entries()
{
	//this->get_const_flags()->print_it();
	auto ret_value = std::vector<local_variable_entry>();

	auto local_table_start = this->localvariable_table_start();
	auto num_entries = *(unsigned short*)this->get_localvariable_table_length_addr();
	auto current_entry = reinterpret_cast<local_variable_table_element*>(local_table_start);
	for (size_t i = 0; i < num_entries; i++)
	{
		ret_value.push_back(current_entry->wrap_to_jvm_variable_entry(this->get_constants()));
		current_entry = reinterpret_cast<local_variable_table_element*>((uint8_t*)current_entry + local_variable_table_element::get_size());
	}

	return ret_value;
}

auto java_hotspot::method::get_const_method() -> const_method* {
	static VMStructEntry* _constMethod_entry = JVMWrappers::find_type_fields("Method").value().get()["_constMethod"];
	if (!_constMethod_entry) return nullptr;
	return *reinterpret_cast<const_method**>(reinterpret_cast<uint8_t*>(this) + _constMethod_entry->offset);
}

MethodCounters* java_hotspot::method::method_counters()
{
	const auto _method_counters_entry = VMWrapper::as("Method", this);
	if (_method_counters_entry == nullptr) {
		throw std::runtime_error("Method not found");
	}
	return _method_counters_entry->get_field<MethodCounters*>("_method_counters");
}

void java_hotspot::method::set_method_counters(MethodCounters* counters)
{
	const auto _method_counters_entry = VMWrapper::as("Method", this);
	if (_method_counters_entry == nullptr) {
		throw std::runtime_error("Method not found");
	}
	_method_counters_entry->set_field<MethodCounters*>("_method_counters", counters);
}

auto java_hotspot::method::get_signature() -> std::string {
	const auto const_method = this->get_const_method();
	const auto signature_index = const_method->get_signature_index();
	const auto const_pool = const_method->get_constants();
	if (!const_pool)
	{
		return "";
	}
	const auto base = reinterpret_cast<symbol**>(const_pool->get_base());
	return base[signature_index]->to_string();
}

auto java_hotspot::method::get_name() -> std::string {
	const auto const_method = this->get_const_method();
	const auto name_index = const_method->get_name_index();
	const auto const_pool = const_method->get_constants();
	const auto base = reinterpret_cast<symbol**>(const_pool->get_base());
	return base[name_index]->to_string();
}

auto java_hotspot::method::get_i2i_entry() -> interpreter_entry* {
	static VMStructEntry* _i2i_entry = JVMWrappers::find_type_fields("Method").value().get()["_i2i_entry"];
	if (!_i2i_entry) return nullptr;
	return *reinterpret_cast<interpreter_entry**>(reinterpret_cast<uint8_t*>(this) + _i2i_entry->offset);
}
#include <base/jvm/hotspot/classes/instance_klass.h>
#include <hotspot/utility/vm_function.h>
void java_hotspot::method::set_breakpoint(uintptr_t index, java_runtime::bytecodes origin_code)
{
	const auto holder_klass = reinterpret_cast<java_hotspot::instance_klass*>(get_const_method()->get_constants()->get_pool_holder());
	const auto bp = jvm_internal::breakpoint_info::create(this, index);
	if (origin_code != java_runtime::bytecodes::_nop) {
		bp->set_orig_bytecode(origin_code);
	}
	bp->set_next(holder_klass->get_breakpoints());
	holder_klass->set_breakpoints(bp);
	const auto counters = method_counters();
	if (counters == nullptr) {
		const auto current = java_hotspot::java_thread::current();
		const auto old_state = current->get_thread_state();

		current->set_thread_state(_thread_in_Java);
		const auto new_counters = vm_function::build_method_counters(current, this);
		current->set_thread_state(old_state);
		if (new_counters)
		{
			new_counters->set_number_of_breakpoints(1u);
			set_method_counters(new_counters);
		}

		return;
	}

	counters->set_number_of_breakpoints(counters->number_of_breakpoints() + 1);
}

auto java_hotspot::method::set_break_point(const uintptr_t offset, const native_callback_t& callback) -> void {
	jvm_break_points::set_breakpoint(this, offset, callback);
}

auto java_hotspot::method::remove_break_point(const uintptr_t offset) -> void {
	jvm_break_points::remove_breakpoint(this, offset);
}

auto java_hotspot::method::remove_all_break_points() -> void {
	jvm_break_points::remove_all_breakpoints(this);
}

auto java_hotspot::method::get_access_flags() -> jvm_internal::access_flags* {
	static VMStructEntry* _access_flags_entry = JVMWrappers::find_type_fields("Method").value().get()["_access_flags"];
	if (!_access_flags_entry) {
		std::cerr << "Access flags not found" << std::endl;
		return {};
	}
	return reinterpret_cast<jvm_internal::access_flags*>(reinterpret_cast<uint8_t*>(this) +
		_access_flags_entry->offset);
}


auto java_hotspot::method::get_flags() -> unsigned short*
{
	if (!this) return nullptr;
	static VMStructEntry* vm_entry = JVMWrappers::find_type_fields("Method").value().get()["_flags"];
	if (!vm_entry)
		return nullptr;
	return (unsigned short*)((uint8_t*)this + vm_entry->offset);
}



auto java_hotspot::method::set_dont_inline(bool enabled) -> void
{
	unsigned short* _flags = get_flags();
	if (!_flags)
	{
		static VMStructEntry* vm_entry = JVMWrappers::find_type_fields("Method").value().get()["_intrinsic_id"];
		if (!vm_entry) return;
		constexpr uintptr_t relative_offset_from_intrinsic_id = 1;
		unsigned char* flags = ((uint8_t*)this + vm_entry->offset + relative_offset_from_intrinsic_id);
		if (enabled)
			*flags |= (1 << 3);
		else
			*flags &= ~(1 << 3);
		return;
	}

	if (enabled)
		*_flags |= _dont_inline;
	else
		*_flags &= ~_dont_inline;
}


