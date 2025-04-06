#include <Windows.h>
#include <base/jvm/hotspot/java_hook.h>
#include <utils/jvm_utility.h>

static void set_up_mc_classloader(JNIEnv* env) {


	auto class_loader = utils::jvm_utility::get_thread_classloader(env, "Render thread");
	class_loader = env->NewGlobalRef(class_loader);
	jni::_refs_to_delete.push_back(class_loader);
	jni::set_custom_find_class([class_loader](const char* class_name) -> jclass {

		static jclass classloader_class = jni::get_env()->FindClass("java/lang/ClassLoader");
		static auto findclass_mid = jni::get_env()->GetMethodID(classloader_class, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

		auto modify_class_name = std::string(class_name);
		size_t pos = modify_class_name.find("/");
		while (pos != std::string::npos) {
			modify_class_name.replace(pos, 1, ".");
			pos = modify_class_name.find("/", pos + 1);
		}

		auto klass_name = jni::get_env()->NewStringUTF(modify_class_name.c_str());
		auto klass = (jclass)jni::get_env()->CallObjectMethod(class_loader, findclass_mid, klass_name);

		jni::get_env()->DeleteLocalRef(klass_name);

		if (klass) return klass;

		return jni::get_env()->FindClass(class_name);

		});
}

static void bootstrap() {
	jni::init();
	auto env = utils::jvm_utility::get_current_thread_env();
	jni::set_thread_env(env);
	auto jni_tools = utils::jvm_utility::get_jni_tools(env);
	auto vmti = std::get<0>(jni_tools);
	jvmtiCapabilities capabilities = { .can_retransform_classes = 1 };
	auto err = vmti->AddCapabilities(&capabilities);
	java_hook::jvm::init(env);
	set_up_mc_classloader(env);

	//开源一个我一直以来开发jni cheat都在用/维护的字节码hook方案，大概从开始开发到现在有一年了 相对来说我都调教到我能实现的最最稳定的效果
	//现在大部分full native 实现的jvm 方法 hook都是修改/hook i2i entry之类的，实际上兼容性很差 很多情况下会出现多个方法用同一个i2i entry的。虽然我这个hook 方案也很吃jvm兼容，但是实际上目前大部分hotspot vm应该都能适配。
	{

		utils::jvm_utility::local_frame frame(env, 99);
		auto gamerenderer_klass = jni::_custom_find_class("net/minecraft/client/renderer/GameRenderer");
		auto bobHurt_mid = env->GetMethodID(gamerenderer_klass, "m_109117_", "(Lcom/mojang/blaze3d/vertex/PoseStack;F)V");
		auto bobHurt_method = utils::jvm_utility::general_resolve(vmti, bobHurt_mid);

		auto const_method = bobHurt_method->get_const_method();
		if (*reinterpret_cast<std::uint8_t*>(const_method->get_bytecode_start()) == static_cast<std::uint8_t>(java_runtime::bytecodes::_aload_0))
		{
			//这里_fast_aload_0是jvm解释执行_aload_0时，会将方法的字节码改成这个，类似一个优化
			//这里做判断主要是为了防止下面的hook代码在方法没被解释之前就执行，然后jvm还没有做这个优化 所以我们给他改成_fast_aload_0，到时候jvm就不会修改了
			*reinterpret_cast<std::uint8_t*>(const_method->get_bytecode_start()) = static_cast<std::uint8_t>(java_runtime::bytecodes::_fast_aload_0);
		}

		auto opcode = static_cast<java_runtime::bytecodes>(bobHurt_method->get_const_method()->get_bytecode().front());
		bobHurt_method->set_breakpoint(0, opcode);
		jvm_break_points::set_breakpoint_with_original_code(bobHurt_method, 0, static_cast<std::uint8_t>(opcode), [](break_point_info* info)
			{
				//这边就是一个hook的handler，我在这里做了一个force return 处理，类似让jvm去执行下面的_return
				//这样就可以实现一个no hurtcam
				info->set_spoofed_bytecode(java_runtime::bytecodes::_return);

				//同时也可以去访问/修改参数。
				// 0 : this ; 1 : PoseStack ; 2 : tickDelta(ticks per frame)
				auto tickDelta = *reinterpret_cast<float*>(info->get_parameter(2));
				std::cout << "tickDelta :" << tickDelta << std::endl;
			});

	}
	//然后注入看看效果






}

static void create_console(void)
{

	FreeConsole();
	if (!AllocConsole())
	{
		char buffer[1024] = { 0 };
		sprintf_s(buffer, "Failed to AllocConsole( ), GetLastError( ) = %d", GetLastError());
		MessageBoxA(HWND_DESKTOP, buffer, "Error", MB_OK);

		return;
	}

	FILE* fp = nullptr;
	freopen_s(&fp, "CONOUT$", "w", stdout);

	*(__acrt_iob_func(1)) = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);

}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		create_console();
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(bootstrap), nullptr, 0, nullptr);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

