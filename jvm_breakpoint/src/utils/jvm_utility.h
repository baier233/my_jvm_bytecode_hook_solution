#pragma once
#include <string_view>
#include "jni/jvmti.h"
#include "jni/jni.h"
#include <tuple>

#include <hotspot/classes/c_method.h>


namespace utils::jvm_utility
{

	class local_frame
	{
	public:
		local_frame(JNIEnv* env, size_t capacity);
		~local_frame();

	private:
		JNIEnv* env = nullptr;
	};

	inline local_frame::local_frame(JNIEnv* env, size_t capacity) :env(env)
	{
		env->PushLocalFrame(capacity);
	}

	inline local_frame::~local_frame()
	{
		env->PopLocalFrame(nullptr);
	}

	void pause_all_java_threads(JNIEnv* env, jvmtiEnv* vmti);
	void resume_all_java_threads(JNIEnv* env, jvmtiEnv* vmt);
	auto get_jni_tools(JNIEnv* env) -> std::tuple<jvmtiEnv*, JavaVM*>;
	auto get_current_thread_env() -> JNIEnv*;
	auto get_thread_classloader(JNIEnv* jni_env, const std::string_view& thread_name) -> jobject;
	auto get_java_vm() -> JavaVM*;
	auto general_resolve(jvmtiEnv* vmti, jmethodID mid) -> java_hotspot::method*;
}
