#pragma once

#include <Windows.h>
#include <iomanip>
#include <Psapi.h>


#define XASSERT(x) if (x) MessageBoxA(HWND_DESKTOP, _(#x), _("FATAL ERROR"), MB_ICONERROR)
namespace utils{
	struct scoped_vprotect {
		scoped_vprotect(void* addr, size_t size, DWORD np) : addr(addr), size(size) { VirtualProtect(addr, size, np, &op); }
		~scoped_vprotect() { VirtualProtect(addr, size, op, &op); }

		void* addr;
		size_t size;
		DWORD op;
	};

	class pattern
	{
	public:
		static uintptr_t find(const char* module, const char* pattern);
		static uintptr_t find(uintptr_t module, const char* pattern);

	public:
		static uintptr_t get_absolute_addr(uintptr_t address, int offset, int size);
	};


}
