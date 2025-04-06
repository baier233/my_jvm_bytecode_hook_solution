#include "java_hook.h"



#pragma comment(lib, "jvm.lib")
/* JVM exports some symbols that we need to use */
extern "C" JNIIMPORT VMStructEntry * gHotSpotVMStructs;
extern "C" JNIIMPORT VMTypeEntry * gHotSpotVMTypes;
extern "C" JNIIMPORT VMIntConstantEntry * gHotSpotVMIntConstants;
extern "C" JNIIMPORT VMLongConstantEntry * gHotSpotVMLongConstants;

#include <log/LOG.h>
#include "utility/vm_function.h"
#include <utils/pattern.h>
static auto init_global_offsets() -> void {
	/* .\hotspot\src\share\vm\classfile\javaClasses.hpp -> class java_lang_Class : AllStatic */
	const auto java_lang_Class = JVMWrappers::find_type_fields("java_lang_Class");
	if (!java_lang_Class.has_value()) {
		LOG("Failed to find java_lang_Class");
	}

	/* java_lang_Class -> _klass_offset */
	global_offsets::klass_offset = *static_cast<jint*>(java_lang_Class.value().get()["_klass_offset"]->address);

	std::string java_thread_pattern = R"(65 48 8B 04 25 58 00 00 00 8B 0D ? ? ? ? 48 8B)";
	auto thread_current_address = utils::pattern::find(reinterpret_cast<uintptr_t>(GetModuleHandle(L"jvm.dll")), java_thread_pattern.c_str()) + 9;
	const auto tls_index_mov = *reinterpret_cast<uint32_t*>(thread_current_address + 2);
	global_offsets::tls_index_address = reinterpret_cast<int*>(thread_current_address + 6 + tls_index_mov);
}



/* Scan bytecode offset */
static auto scan_bytecode_offset(JNIEnv* env) -> void {
	/* Any class */
	const auto integer_klass = env->FindClass("java/lang/Integer");

	if (!integer_klass) {
		LOG("Failed to find java/lang/Integer");
	}
	/* Any method */
	const auto integer_hash_code = env->GetMethodID(integer_klass, "shortValue", "()S");
	if (!integer_hash_code) {
		LOG("Failed to find java/lang/Integer::shortValue");
	}
	/* Get method */
	const auto hash_method = *reinterpret_cast<java_hotspot::method**>(integer_hash_code);
	if (!hash_method) {
		LOG("Failed to find java/lang/Integer::shortValue");
	}
	java_hotspot::bytecode_start_offset = java_hotspot::const_method::get_const_method_length();
	LOG("Bytecode start offset: " << java_hotspot::bytecode_start_offset);

	/* Interception address */
	java_hotspot::interpreter_entry* interpreter_entry = hash_method->get_i2i_entry();
	if (const auto interception_address = interpreter_entry->get_interception_address(); !interception_address) {
		LOG("Failed to find interception address");
	}
	LOG("Interception address: " << reinterpret_cast<void*>(interpreter_entry->get_interception_address()));

	/* Get dispatch table */
	const uintptr_t dispatch_table = *reinterpret_cast<uintptr_t*>(interpreter_entry->get_interception_address() + 2);
	if (!dispatch_table) {
		LOG("Failed to find dispatch table");
	}
	LOG("Dispatch table: " << reinterpret_cast<void*>(dispatch_table));

	/* Get breakpoint method */
	const uintptr_t breakpoint_method = *reinterpret_cast<uintptr_t*>(dispatch_table + static_cast<uint8_t>(java_runtime::bytecodes::_breakpoint) * 8);

	const auto if_icmp_method =
		*reinterpret_cast<uintptr_t*>(dispatch_table + static_cast<uint8_t>(java_runtime::bytecodes::_if_icmple) * 8);
	const std::vector<void*> if_icmp_calls = vm_helper::find_vm_calls(reinterpret_cast<void*>(if_icmp_method));
	vm_function::build_method_counters = static_cast<vm_function::BuildMethodCountersType>(if_icmp_calls[0]);

	const uintptr_t aload_method = *reinterpret_cast<uintptr_t*>(dispatch_table + static_cast<uint8_t>(java_runtime::bytecodes::_aload) * 8);
	if (!breakpoint_method) {
		LOG("Failed to find breakpoint method");
	}
	LOG("Breakpoint method: " << reinterpret_cast<void*>(breakpoint_method));
	LOG("Aload method: " << reinterpret_cast<void*>(aload_method));
	/* Get VM calls */

	const std::vector<void*> vm_calls = vm_helper::find_vm_calls(reinterpret_cast<PVOID>(breakpoint_method));
	if (vm_calls.size() < 2) {
		LOG("Failed to find VM calls");
	}
	LOG("VM calls: " << vm_calls.size());

	// Hook the runtime methods
	void* runtime_get_original_bytecode = vm_calls[0];
	void* runtime_breakpoint_method = vm_calls[1];
	LOG("Runtime get original bytecode: " << runtime_get_original_bytecode);
	LOG("Runtime breakpoint method: " << runtime_breakpoint_method);
	jvm_break_points::breakpoint_hook.InitHook(runtime_breakpoint_method, jvm_break_points::breakpoint_callback);
	jvm_break_points::original_bytecode_hook.InitHook(runtime_get_original_bytecode,
		jvm_break_points::original_bytecode_handler);
	jvm_break_points::breakpoint_hook.SetHook();
	jvm_break_points::original_bytecode_hook.SetHook();
}




bool java_hook::jvm::init(JNIEnv* env, bool breakpoint)
{
	//const auto interop = std::make_unique<java_interop>(env);
	JVMWrappers::init(gHotSpotVMStructs, gHotSpotVMTypes, gHotSpotVMIntConstants, gHotSpotVMLongConstants);
	init_global_offsets();
	if (breakpoint)
	{
		scan_bytecode_offset(env);
	}
	return true;
}

bool java_hook::jvm::clean()
{
	jvm_break_points::breakpoint_hook.RemoveHook();
	jvm_break_points::original_bytecode_hook.RemoveHook();
	return true;
}
