#pragma once
#include "include_header.h"
#include "utility/jvm_internal.h"
#include "break/jvm_break_points.h"
#include "utility/vm_helper.h"
#include "classes/c_method.h"
#include "classes/java_thread.h"
#include "classes/instance_klass.h"
#include "classes/const_pool.h"


#include <ranges>
#include <algorithm>

namespace java_hook {
	namespace jvm {
		bool init(JNIEnv* env, bool breakpoint = true);
		bool clean();
	}

}

struct bytecode_info
{
	bytecode_info() {  };
	java_runtime::bytecodes opcode{};
	std::vector<int> operands{};
	int index = 0;
};