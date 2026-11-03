#pragma once

// this .h file is a toolkit for lots of the obfuscation done in rose.exe

#include <iostream>
#include <Windows.h>
#include <winternl.h>
#include <thread>

#include "strcryptx.hpp"
#include "strc.h"
#include "inline_asm.h"
#include "mem.h"
#include "sigs.hpp"

#define __exceptf __except (EXCEPTION_EXECUTE_HANDLER)

HMODULE ntdll_handle = GetModuleHandle("ntdll.dll");
LPVOID image_base = GetModuleHandleA(NULL);

// this is just a whole bunch of filler string of AT LEAST 4096 bytes
// this is just the buffer which nukes some code in the chicken level to generate an exception for an SEH hook
// honestly i think it was an excuse for me to just spam keys on my keyboard for a while
#pragma region random 4096
const char* random4096 = "\x64\x48\x8B\x04\x25\x00\x00\x00\x00patchmem(nuke_addr, WEROIKLMFDSMMMMMM ? MMMMMMM ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? MM_)IXC_)VI@#$(*U#@ * U(R$E * (#RU@E$WW#RP * @(EUJWP * (UR#@EW#@R$KJ"
"NKJN#R@$KJN#RKJN#R$@KJN@#$KJN@#$KJN@#$KJN@#$KJN#@ @#$KJN@#$KJN@#$KJN@#$KJN@#$KJNKJN@#$KJNKJN$#@NJK@#$KJNKJN$#@KJNJNK$#@KJN@#$KJNKNJ$#@NKJNKJ@#$KJN$@#NKJ#$@KJN#@$JKN#$@KJN$#@JKNJNK@$#KJNKNJ$#@KJN@#$KJN@#$LKJN@#$KLJN@#$LKJN@#$KLJN@#LKJN@#$LKJN@#$KLJ"
"N@#$$KLJN@#$$KLJN@#$KLJN@#$KJN@#$KJN#$@KNJ#@$KNJ@#$JNK#@$JNK@#$JNK#@$JNK#@$JNK#@$JKN@$#NKJ#@$KJN#@$KJN#$@KJN@#$LNKJ@#N$ > <MN@#$><MN#><MN><M>M<XCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXC"
"V><MXCV><MXCV><MXCV><MXCV><MXCV> < MXCV)M?M?M?M?M?MM_)IXC_)VI@#$(*U#@*U(R$E*(#RU@E$WW#RP*@(EUJWP*(UR#@EW#@R$KJNKJN#R@$KJN#RKJN#R$@KJN@#$KJN@#$KJN@#$KJN@#$KJN#@$KJN@#$KJN@#$KJN@#$KJN@#$KJN@#$KJNKJN@#$KJNKJN$#@NJK@#$KJNKJN$#@KJNJNK$#@KJN@#$KJNKNJ$#@"
"NKJNKJ@#$KJN$@#NKJ#$@KJN#@$JKN#$@KJN$#@JKNJNK@$#KJNKNJ$#@KJN@#$KJN@#$LKJN@#$KLJN@#$LKJN@#$KLJN@#LKJN@#$LKJN@#$KLJN@#$$KLJN@#$$KLJN@#$KLJN@#$KJN@#$KJN#$@KNJ#@$KNJ@#$JNK#@$JNK@#$JNK#@$JNK#@$JNK#@$JKN@$#NKJ#@$KJN#@$KJN#$@KJN@#$LNKJ@#N$><MN@#$><MN#><M"
"N><M>M<XCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><1111111111111111?MMMMMMM?M?M?M?M?M?M?M?M?M?M?M?M?		patchmem(nuke_addr, WEROIKLMFDSM1111ROSE.EXEROSE"
".EXE    ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? M ? MM_)IXC_)VI@#$(*U#@ * U(R$E * (#RU@E$WW#RP * @(EUJWP * (UR#@EW#@R$KJNKJN#R@$KJN#RKJN#R$@KJN@#$KJN@#$KJN@#$KJN@#$KJN#@$KJN@#$KJN@#$KJN@#$KJN@#$KJN@#$KJNKJN@#$KJNKJN$#@NJK@#$K"
"JNKJN$#@KJNJNK$#@KJN@#$KJNKNJ$#@NKJNKJ@#$KJN$@#NKJ#$@KJN#@$JKN#$@KJN$#@JKNJNK@$#KJNKNJ$#@KJN@#$KJN@#$LKJN@#$KLJN@#$LKJN@#$KLJN@#LKJN@#$LKJN@#$KLJN@#$$KLJN@#$$KLJN@#$KLJN@#$KJN@#$KJN#$@KNJ#@$KNJ@#$JNK#@$JNK@#$JNK#@$JNK#@$JNK#@$JKN@$#NKJ#@$KJN#@$KJN"
"#$@KJN@#$LNKJ@#N$ > <MN@#$><MN#><MN><M>M<XCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV> < MXCV)M?M?M?M?M?MM_)IXC_)VI@#$(*U#@*U(R$E*(# 1111 1111 1?11 1111 111"
"E <1113 RU@E$WW#RP*@(EUJWP*(UR#@EW#@R$KJNKJN#R@$KJN#RKJN#R$@KJN@#$KJN@#$KJN@#$KJN@#$KJN#@$KJN@#$KJN@#$KJN@#$KJN@#$KJN@#$KJNKJN@#$KJNKJN$#@NJK@#$KJNKJN$#@KJNJNK$#@KJN@#$KJNKNJ$#@NKJNKJ@#[CONFIDENTIAL] [REDACTED]KJN$#@[ROSE.EXE]3@$#KJNKN            "
"J$#@KJN@#$KJN@#$LKJN@#$KLJN@#$LKJN@#$KLJN@#LKJN@#$LKJN@#$KLJN@#$$KLJN@#$$KLJN@#$KLJN@#$KJN@#$KJN#$@KNJ#@$KNJ@#$JNK#@$JNK@#$JNK#@$JNK#@$JNK#@$JKN@$#NKJ#@$KJN#@$KJN#$@KJN@#$LNKJ@#N$><MN@#$><MN#><MN><M>M<XCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV>"
"<MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><ROSE.EXE  1111!><YOU GOT THIS!!!><MXCV><MXCV><MXCV><MXCV><MXCV><MXCVWEROIKLMFDSMMMMMM?MMMMMMM?M?M?M?M?M?M?M?M?M?M?M?M?		patchmem(nuke_addr, WEROIKLMFDSMMMMMM ? MMMMMMM ? M ? M ? M ? M ? M "
"? M ? M ? M ? M ? M ? MM_)IXC_)VI@#$(*U#@ * U(R$E * (#RU@E$WW#RP * @(EUJWP * (UR#@EW#@R$KJNKJN#R@$KJN#RKJN#R$@KJN@#$KJN@#$KJN@#$KJN@#$KJN#@$KJN@#$KJN@#$KJN@#$KJN@#$KJN@#$KJNKJN@#$KJNKJN$#@NJK@#$KJNKJN$#@KJNJNK$#@KJN@#$KJNKNJ$#@NKJNKJ@#$KJN$@#NKJ#$"
"@KJN#@$JKN#$@KJN$#@JKNJNK@$#KJNKNJ$#@KJN@#$KJN@#$LKJN@#$KLJN@#$LKJN@#$KLJN@#LKJN@#$LKJN@#$KLJN@#$$KLJN@#$$KLJN@#$KLJN@#$KJN@#$KJN#$@KNJ#@$KNJ@#$JNK#@$JNK@#$JNK#@$JNK#@$JNK#@$JKN@$#NKJ#@$KJN#@$KJN#$@KJN@#$LNKJ@#N$ > <MN@#$><MN#><MN><M>M<XCV><MXCV><"
"MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV> < MXCV)M?M?M?M?M?MM_)IXC_)VI@#$(*U#@*U(R$E*(#RU@E$WW#RP*@(EUJWP*(UR#@EW#@R$KJNKJN#R@$KJN#RKJN#R$@KJN@#$KJN@#$KJN@#$KJN@#$"
"KJN#@$KJN@#$KJN@#$KJN@#$KJN@#$KJN@#$KJNKJN@#$KJNKJN$#@NJK@#$KJNKJN$#@KJNJNK$#@KJN@#$KJNKNJ$#@NKJNKJ@#$KJN$@#NKJ#$@KJN#@$JKN#$@KJN$#@JKNJNK@$#KJNKNJ$#@KJN@#$KJN@#$LKJN@#$KLJN@#$LKJN@#$KLJN@#LKJN@#$LKJN@#$KLJN@#$$KLJN@#$$KLJN@#$KLJN@#$KJN@#$KJN#$@KN"
"J#@$KNJ@#$JNK#@$JNK@#$JNK#@$JNK#@$JNK#@$JKN@$#NKJ#@$KJN#@$KJN#$@KJN@#$LNKJ@#N$><MN@#$><MN#><MN><M>M<XCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><MXCV><ROSE.EXE      <MXCV><MXCV><MXCV><MXCV><MXCV";
#pragma endregion

// import address table
namespace IAT
{
	void hook(const char* func, uintptr_t hook_addr, uintptr_t* func_addr = nullptr)
	{
		PIMAGE_DOS_HEADER dosHeaders = (PIMAGE_DOS_HEADER)image_base;
		PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)image_base + dosHeaders->e_lfanew);

		PIMAGE_IMPORT_DESCRIPTOR importDescriptor = NULL;
		IMAGE_DATA_DIRECTORY importsDirectory = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(importsDirectory.VirtualAddress + (DWORD_PTR)image_base);
		LPCSTR libraryName = NULL;
		HMODULE library = NULL;
		PIMAGE_IMPORT_BY_NAME functionName = NULL;


		while (importDescriptor->Name != NULL)
		{
			libraryName = (LPCSTR)importDescriptor->Name + (DWORD_PTR)image_base;
			library = LoadLibraryA(libraryName);

			if (library)
			{
				PIMAGE_THUNK_DATA originalFirstThunk = NULL, firstThunk = NULL;
				originalFirstThunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)image_base + importDescriptor->OriginalFirstThunk);
				firstThunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)image_base + importDescriptor->FirstThunk);

				while (originalFirstThunk->u1.AddressOfData != NULL)
				{
					functionName = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)image_base + originalFirstThunk->u1.AddressOfData);

					// find func address
					if (std::string(functionName->Name).compare(func) == 0)
					{
						SIZE_T bytesWritten = 0;
						DWORD oldProtect = 0;
						VirtualProtect((LPVOID)(&firstThunk->u1.Function), 8, PAGE_READWRITE, &oldProtect);

						// swap addresses
						if (func_addr != nullptr) {
							*func_addr = firstThunk->u1.Function;
						}
						firstThunk->u1.Function = (DWORD_PTR)hook_addr;

						DWORD dummy0;
						VirtualProtect((LPVOID)(&firstThunk->u1.Function), 8, oldProtect, &dummy0);

						return;
					}
					originalFirstThunk++;
					firstThunk++;
				}
			}

			importDescriptor++;
		}
	}
	void unhook(const char* func, uintptr_t func_addr)
	{
		hook(func, func_addr);
	}

	__declspec(noinline) void hookc(const char* func, uintptr_t hook_addr, uintptr_t* func_addr = nullptr) 
	{
		std::string x = func;
		for (size_t i = 0; i < x.length(); i++)
		{
			x[i] ^= 0x96;
		}
		hook(x.c_str(), hook_addr, func_addr);
	}
	__declspec(noinline) void unhookc(const char* func, uintptr_t func_addr) 
	{
		std::string x = func;
		for (size_t i = 0; i < x.length(); i++)
		{
			x[i] ^= 0x96;
		}
		unhook(x.c_str(), func_addr);
	}
}

// patch code at runtime (can only modify inline_asm reliably)
class patchkit {
public:
	patchkit(uint32_t id, uint32_t size, uintptr_t func)
	{
		addr = find_signature(func, id + 0x1000, 1) + sig_size;

		for (size_t i = 0; i < size; i++)
		{
			original_code.push_back(*(uint8_t*)(i + addr));
		}
	}
	patchkit(uintptr_t address, uint32_t size) 
	{
		addr = address;
		if (addr) {
			for (size_t i = 0; i < size; i++)
			{
				original_code.push_back(*(uint8_t*)(i + addr));
			}
		}
	}
	~patchkit()
	{
		original_code.~vector();
	}

	void patch(const char* bytes)
	{
		patchmem(addr, bytes, original_code.size());
	}

	void nuke()
	{
		std::string bytes = "";
		for (size_t i = 0; i < original_code.size(); i++)
		{
			bytes += (char)((rand() ^ (original_code[i] + 7) + (i * 3)) % 0x100);
		}
		patch(bytes.c_str());
	}

	void restore()
	{
		DWORD old;
		VirtualProtectEx(phandle, (LPVOID)addr, original_code.size(), PAGE_EXECUTE_READWRITE, &old);

		for (size_t i = 0; i < original_code.size(); i++)
		{
			*(uint8_t*)(addr + i) = original_code[i];
		}

		DWORD dummy0;
		VirtualProtectEx(phandle, (LPVOID)addr, original_code.size(), old, &dummy0);
	}

	uintptr_t addr;
	std::vector<char> original_code;
};

// integer encryption and obfuscation
namespace ints
{
	template<typename T>
	constexpr T c_encrypt(T integer) {
		return ((integer ^ 0xEEF0394) + 64) ^ 0x7C93;
	}
	
	template<typename T>
	T r_encrypt(T integer) {
		// force runtime eval
		volatile T x = integer;
		x = ((x ^ 0xEEF0394) + 64) ^ 0x7C93;
		return x;
	}

	template<typename T>
	T decrypt(T integer) {
		// force runtime eval
		volatile T x = integer;
		x = ((x ^ 0x7C93) - 64) ^ 0xEEF0394;
		return x;
	}

	template<typename T>
	constexpr T hash(T integer)
	{
		return (integer & 0x7ff9f) + (integer & 0x948333) + (integer & 0x8394) + (integer << 1);
	}

	template<typename T>
	T obf(T integer)
	{
		return decrypt(c_encrypt(integer));
	}
}

// structured exception handling abuse util
namespace SEH
{
	// put exception-causers in a __try __except(1) block if doing any SEH hooking
	// like this
	/*
	__try {
		trigger_exception();
	}
	__except(1) {
		std::cout << "this code will never be reached if hooked\n";
	}
	*/

	// hooks the restore address
	namespace indirect 
	{
		// put this in a __try __except(1)
		void trigger_exception() {
			RaiseException(STATUS_INTEGER_DIVIDE_BY_ZERO, 0, 0, NULL);
		}

		const char hook_bytes[] = "\xE9\x26\x86\x07\x00\x90\x90\x90\x90\x90\x90";
		char cc_bytes[] = "\x48\x8B\x41\x78\x48\x8B\x91\x88\x00\x00\x00\x50\x48\xB8\x26\x10\xF7\x10\xF7\x7F\x00\x00\x48\x89\x44\x24\x08\x58\xE9\xBF\x79\xF8\xFF";
		//                                                                           xx  xx  xx  xx  xx  xx  xx  xx                                 
		constexpr uint32_t hook_size = 11;
		constexpr uint32_t cc_size = 33;

		const char handler_sig[] = "\x48\x8B\x41\x78\x48\x8B\x91\x88\x00\x00\x00\x4C\x8B\x81\xB8\x00\x00\x00\x4C\x8B\x89\xC0\x00\x00\x00\x4C\x8B\x91\xC8\x00\x00\x00\x4C\x8B\x99\xD0\x00\x00\x00\x48\x8B\x99\x90\x00\x00\x00\x48\x8B\xB1\xA8\x00\x00\x00\x48\x8B\xB9\xB0\x00\x00\x00\x48\x8B\xA9\xA0\x00\x00\x00\x4C\x8B\xA1\xD8\x00\x00\x00\x4C\x8B\xA9\xE0\x00\x00\x00\x4C\x8B\xB1\xE8\x00\x00\x00\x4C\x8B\xB9\xF0\x00\x00\x00\x48\x8B\x89\x80\x00\x00\x00\x48\xCF";
		const char handler_mask[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
		constexpr uint32_t cc_offset = 0x07862B;

		patchkit jmp_patch(0, 0);
		patchkit cc_patch(0, 0);

		uintptr_t hook_addr = 0;

		// remember inline_asm sigs are id + 0x1000
		void hook(uint32_t id, uintptr_t func)
		{
			uintptr_t jmp_addr = find_signature(func, id, 1);
			if (hook_addr == 0) {
				hook_addr = internal_pattern_scan((uintptr_t)ntdll_handle, handler_sig, handler_mask, 104, 1, 0x10000000);
			}

			jmp_patch.patchkit::patchkit(hook_addr, hook_size);
			jmp_patch.patch(hook_bytes);

			// reverse endian
			for (int i = 7; i >= 0; i--)
			{
				cc_bytes[14 + i] = (jmp_addr & ((uint64_t)0xFF << (8 * i))) >> (8 * i);
			}

			cc_patch.patchkit::patchkit(hook_addr + cc_offset, cc_size);
			cc_patch.patch(cc_bytes);
		}

		// you don't *need* to unhook after hooking, a hook can replace another without unhooking
		void unhook()
		{
			// make sure that it was the actual normal bytes, and not just previous bytes
			// this is so that you don't always need to unhook after hooking :)
			for (size_t i = 0; i < jmp_patch.original_code.size(); i++)
			{
				jmp_patch.original_code[i] = handler_sig[i];
			}
			jmp_patch.restore();

			// every byte in the code cave is null before modifying
			for (size_t i = 0; i < cc_patch.original_code.size(); i++)
			{
				cc_patch.original_code[i] = '\x00';
			}
			cc_patch.restore();
		}
	}

	// patches the very start of the handler with a jmp
	namespace direct 
	{
		uintptr_t hook_addr = 0;

		char standard_bytes[11] = {};
		
		const char sig[] = "\xFC\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0";
		const char mask[] = "xxxx????xxx";

		patchkit patch(0, 0);

		uint64_t get_rsp() {
			CONTEXT context;
			RtlCaptureContext(&context);
			return context.Rsp;
		}

		void hook(uint32_t id, uintptr_t func, uint64_t rsp)
		{
			uintptr_t jmp_addr = find_signature(func, id, 1);
			if (!hook_addr) {
				hook_addr = internal_pattern_scan((uintptr_t)ntdll_handle, sig, mask, 11, 1, 0x1000000);
				memcpy(standard_bytes, (void*)hook_addr, 11);
			}

			byte p_bytes[] = "\x48\xB8\x00\x00\x00\x00\x00\x00\x00\x00\x48\xBC\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xE0";

			patch.patchkit::patchkit(hook_addr, sizeof p_bytes - 1);

			// mov rax, x
			for (size_t i = 0; i < 8; i++)
			{
				p_bytes[i + 2] = (jmp_addr >> (i * 8)) & 0xFF;
			}

			// mov rsp, y
			for (size_t i = 0; i < 8; i++)
			{
				p_bytes[i + 12] = (rsp >> (i * 8)) & 0xFF;
			}

			patch.patch((const char*)p_bytes);
		}

		// hooks and triggers exception
		__forceinline void trigger_exception(uint32_t id, uintptr_t func)
		{
			hook(id, func, get_rsp());
			volatile int x = 0;
			x /= x;
		}

		void unhook()
		{
			for (size_t i = 0; i < sizeof standard_bytes - 1; i++)
			{
				patch.original_code[i] = standard_bytes[i];
			}
			patch.restore();
		}
	}

}

// VEH hooking
namespace VEH
{
	// returns handler, use for unhooking
	void* hook(PVECTORED_EXCEPTION_HANDLER func)
	{
		return AddVectoredExceptionHandler(1, func);
	}

	void unhook(void* handler) 
	{
		RemoveVectoredExceptionHandler(handler);
	}

	void trigger_exception() {
		RaiseException(STATUS_INTEGER_DIVIDE_BY_ZERO, 0, 0, NULL);
	}

	LONG WINAPI example_handler(PEXCEPTION_POINTERS pExceptionInfo)
	{
		std::cout << "Vectored Exception Handler called!" << std::endl;
		std::cout << "Exception code: 0x" << std::hex << pExceptionInfo->ExceptionRecord->ExceptionCode << std::endl;

		// pass to SEH to fix RIP
		return EXCEPTION_CONTINUE_SEARCH;
	}

	void example()
	{
		void* handler = hook(example_handler);

		__try {
			trigger_exception();
		}
		__except (1) {
			std::cout << "end\n";
		}

		unhook(handler);
	}
}

// virtual function pointer table hooking
namespace vtable
{
	uintptr_t* get_vtable(void* vclass)
	{
		return *(uintptr_t**)vclass;
	}
	void hook(void* vclass, uint32_t index, void* hook_addr, uintptr_t* original = nullptr)
	{
		uintptr_t* vtable = *(uintptr_t**)vclass;

		DWORD old;
		VirtualProtect(&vtable[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &old);
		if (original != nullptr) {
			*original = vtable[index];
		}
		vtable[index] = (uintptr_t)hook_addr;
		VirtualProtect(&vtable[index], sizeof(void*), old, &old);
	}
	void unhook(void* vclass, uint32_t index, uintptr_t original)
	{
		hook(vclass, index, (void*)original);
	}
}

// return address spoofing
// i DID NOT write this or the .asm file
namespace RAS
{
	namespace detail
	{
		extern "C" void* _spoofer_stub();

		template <typename Ret, typename... Args>
		static inline auto shellcode_stub_helper(
			const void* shell,
			Args... args
		) -> Ret
		{
			auto fn = (Ret(*)(Args...))(shell);
			return fn(args...);
		}

		template <std::size_t Argc, typename>
		struct argument_remapper
		{
			// At least 5 params
			template<
				typename Ret,
				typename First,
				typename Second,
				typename Third,
				typename Fourth,
				typename... Pack
			>
				static auto do_call(
					const void* shell,
					void* shell_param,
					First first,
					Second second,
					Third third,
					Fourth fourth,
					Pack... pack
				) -> Ret
			{
				return shellcode_stub_helper<
					Ret,
					First,
					Second,
					Third,
					Fourth,
					void*,
					void*,
					Pack...
				>(
					shell,
					first,
					second,
					third,
					fourth,
					shell_param,
					nullptr,
					pack...
					);
			}
		};

		template <std::size_t Argc>
		struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>>
		{
			// 4 or less params
			template<
				typename Ret,
				typename First = void*,
				typename Second = void*,
				typename Third = void*,
				typename Fourth = void*
			>
				static auto do_call(
					const void* shell,
					void* shell_param,
					First first = First{},
					Second second = Second{},
					Third third = Third{},
					Fourth fourth = Fourth{}
				) -> Ret
			{
				return shellcode_stub_helper<
					Ret,
					First,
					Second,
					Third,
					Fourth,
					void*,
					void*
				>(
					shell,
					first,
					second,
					third,
					fourth,
					shell_param,
					nullptr
					);
			}
		};
	}


	template <typename Ret, typename... Args>
	static inline auto spoof_call(
		const void* trampoline,
		Ret(*fn)(Args...),
		Args... args
	) -> Ret
	{
		struct shell_params
		{
			const void* trampoline;
			void* function;
			void* rbx;
		};

		shell_params p{ trampoline, reinterpret_cast<void*>(fn) };
		using mapper = detail::argument_remapper<sizeof...(Args), void>;
		return mapper::template do_call<Ret, Args...>((const void*)&detail::_spoofer_stub, &p, args...);
	}
}

// functions for quickly getting tick count
namespace timing
{
	// gets the microseconds
	uint64_t qperfc()
	{
		LARGE_INTEGER ret;
		QueryPerformanceCounter(&ret);
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		ret.QuadPart *= 1000000;
		ret.QuadPart /= freq.QuadPart;
		return ret.QuadPart;
	}
	// gets raw tick
	uint64_t qperfraw()
	{
		LARGE_INTEGER ret;
		QueryPerformanceCounter(&ret);
		return ret.QuadPart;
	}
}

// get hardware serials and unique identifiers
namespace HWID
{
	uint32_t ssd_serial()
	{
		DWORD serial = 0;
		GetVolumeInformation("C:\\", NULL, 0, &serial, NULL, NULL, NULL, 0);
		return serial;
	}

	uint64_t cpu_serial()
	{
		int cpu_info[4] = { 0 };
		__cpuid(cpu_info, 0);

		return cpu_info[0] + ((uint64_t)cpu_info[1] << 32);
	}
}

// custom bytecode
namespace virtualization
{
	class t9 {
		// each letter is +1 the last, *rax = str, set boolean2 to 1
		// "\x09\x04\x0B\x01\x02\x01\x09\x06\x0B\x03\xFF\x00\x08\x0B\x01\x01\xFD\x08\x01\xFB\x09\x01\x07\xE8"

		// letter-2 ^ letter-1 + 5 = letter, *rax = str, set boolean2 to 1
		// "\x09\x05\x03\x02\x0B\x03\xFF\x6A\x08\x01\xFB\x09\x05\x03\x01\x0B\x03\xFF\x21\x08\x01\xFB\x09\x05\x12\x0B\x03\xFF\x00\x08\x22\xF9\x01\xF8\x08\x1A\x09\x05\x12\x06\x01\x09\x03\x05\x01\x0B\x03\x0B\x02\x0F\x06\x03\x05\x09\x04\x13\x0B\x02\xFE\x08\x01\xFB\x11\x07\xD5\x09\x07\xFF\x09\x08\x01\xFB"
	public:
		void zero_registers() {
			rax = 0, rbx = 0, rcx = 0, inc = 0, boolean = 0, boolean2 = 0;
		}

		t9() {
			zero_registers();
		}

		void exec(const char* bytecode)
		{
			for (size_t i = 0; bytecode[i] != '\x00'; i++)
			{
				// std::cout << std::hex << (int)(unsigned char)bytecode[i] << std::dec << '\n';
				switch ((unsigned char)bytecode[i])
				{
				case 1:
					rax += bytecode[i + 1];
					i++;
					break;
				case 2:
					rbx += bytecode[i + 1];
					i++;
					break;
				case 3:
					rcx += bytecode[i + 1];
					i++;
					break;
				case 4:
					rax -= bytecode[i + 1];
					i++;
					break;
				case 5:
					rbx -= bytecode[i + 1];
					i++;
					break;
				case 6:
					rcx -= bytecode[i + 1];
					i++;
					break;
				case 7: // jmp
					i += (int)(char)bytecode[i + 1];
					i++;
					break;
				case 8: // je
					if (boolean) {
						i += (int)(char)bytecode[i + 1];
					}
					i++;
					break;
				case 9: // mov
					switch (bytecode[i + 1])
					{
					case 1:
						rax = rbx;
						break;
					case 2:
						rax = rcx;
						break;
					case 3:
						rbx = rcx;
						break;
					case 4:
						rbx = rax;
						break;
					case 5:
						rcx = rax;
						break;
					case 6:
						rcx = rbx;
						break;
					case 7:
						rcx = inc;
						break;
					}
					i++;
					break;
				case 0x0A: // mov x, qword ptr:[x]
					switch (bytecode[i + 1])
					{
					case 1:
						rax = *(uint64_t*)rax;
						break;
					case 2:
						rbx = *(uint64_t*)rbx;
						break;
					case 3:
						rcx = *(uint64_t*)rcx;
						break;
					}
					i++;
					break;
				case 0x0B: // mov x, byte ptr:[x]
					switch (bytecode[i + 1])
					{
					case 1:
						rax = *(uint8_t*)rax;
						break;
					case 2:
						rbx = *(uint8_t*)rbx;
						break;
					case 3:
						rcx = *(uint8_t*)rcx;
						break;
					}
					i++;
					break;
				case 0x0C:
					rax ^= bytecode[i + 1];
					i++;
					break;
				case 0x0D:
					rbx ^= bytecode[i + 1];
					i++;
					break;
				case 0x0E:
					rcx ^= bytecode[i + 1];
					i++;
					break;
				case 0x0F:
					switch (bytecode[i + 1])
					{
					case 1:
						rax ^= rbx;
						break;
					case 2:
						rax ^= rcx;
						break;
					case 3:
						rbx ^= rcx;
						break;
					case 4:
						rbx ^= rax;
						break;
					case 5:
						rcx ^= rax;
						break;
					case 6:
						rcx ^= rbx;
						break;
					}
					i++;
					break;
				case 0x10:
					boolean = (char)rcx > bytecode[i + 1];
					i++;
					break;
				case 0x11:
					inc++;
					break;
				case 0x12:
					rcx += inc;
					break;
				case 0x13:
					rbx += inc;
					break;
				case 0xF8: // NOT
					boolean ^= 1;
					break;
				case 0xF9:
					boolean = (char)inc > bytecode[i + 1];
					i++;
					break;
				case 0xFA:
					boolean = (char)inc == bytecode[i + 1];
					i++;
					break;
				case 0xFB:
					boolean2 = boolean;
					break;
				case 0xFC:
					boolean = rax == rbx;
					break;
				case 0xFD:
					boolean = rax == rcx;
					break;
				case 0xFE:
					boolean = rbx == rcx;
					break;
				case 0xFF:
					boolean = (char)rcx == bytecode[i + 1];
					i++;
					break;
				case 0x30:
					volatile int xx1 = 0;
					volatile int xx2 = 0;
					if (xx1 != xx2) {
						std::cout << cryptx_obf("It's like assembly, just a simplified version.\nNote down what byte commands do what, then translate all bytes into something readable, then find the key.\n", "h_k");
					}
				}
			}
		}

		uint64_t rax, rbx, rcx, inc, boolean, boolean2;
	};

	namespace t10 {
		class dec {
			virtual void uno(const byte* table, const byte* directions) = 0;
			virtual void dos(const byte* table, const byte* directions) = 0;
			virtual void tres(const byte* table, const byte* directions) = 0;
			virtual void quadro(const byte* table, const byte* directions) = 0;
		};

		class virt : public dec {
		public:
			static constexpr uint32_t reg_count = 20;
			uint64_t r[reg_count] = {};

			// 1
			virtual void uno(const byte* table, const byte* directions) override {
				const byte* bytecode = (const byte*)"\x49\x00\x02\x01\x11\x0f\xff\x2a\x49\x00\x03\x01\x00\x07\x05\x07\x02\x43\x07\x06\x81\x08\x06\x09\x02\x03\x08\xfe\x08\x49\x00\x01\x00\x03\x01\x00\x02\x01\x81\x04\x03\x05\x03\x04\x01\x02\xd9\x04\xfe\x04\x43\x0f\x13\x00\x0f\x01\x43\x0f\x05\x00\x0f\x01\x43\x0f\x09\x00\x0f\x01\x01\x0f\x0e\x04\x0e\x11\x49\x24\x0b\x81\x0d\x0e\x0b\x03\x0d\x01\x02\xaf\x0d\xfe\x0d\x76";

				r[17] = (uint64_t)table;
				r[0] = (uint64_t)directions;
				r[1] = 1;
				for (size_t i = 0; i != 999;)
				{
					inline_asm(0x1000, 7);
					switch (bytecode[i] ^ 1) {
					case 0: // mov r[x], r[y] (swapped)
						r[bytecode[i + 2]] = r[bytecode[i + 1]];
						break;
					case 1: // add r[x], ??
						r[bytecode[i + 1]] += bytecode[i + 2];
						break;
					case 2: // xor r[x], ??
						r[bytecode[i + 1]] ^= bytecode[i + 2];
						break;
					case 3:	// mul r[x], ?? (swapped)
						r[bytecode[i + 2]] *= bytecode[i + 1];
						break;
					case 4: // add r[x], r[y]
						r[bytecode[i + 1]] += r[bytecode[i + 2]];
						break;
					case 5: // sub r[x], r[y]
						r[bytecode[i + 1]] -= r[bytecode[i + 2]];
						break;
					case 0x42: // mov r[x], byte ptr r[y] (swapped)
						r[bytecode[i + 2]] = *(byte*)r[bytecode[i + 1]];
						break;
					case 0x77:
						i = 999 - 3;
						break;
					case 0x48: // mov r[x], ?? (swapped)
						r[bytecode[i + 2]] = bytecode[i + 1];
						break;
					case 0x80: // cmp r[x], r[y], r[z]
						r[bytecode[i + 1]] = r[bytecode[i + 2]] == r[bytecode[i + 3]];
						i++;
						break;
					case 0xFE: // jmp ??
						i += bytecode[i + 1];
						i--;
						break;
					case 0xFF: // jmp r[x]
						i += (char)r[bytecode[i + 1]];
						i--;
						break;
					}
					i += 3;
				}
			}
			// 3
			virtual void dos(const byte* table, const byte* directions) override {
				const byte* bytecode = (const byte*)"\x4b\x00\x02\x03\x11\x0f\xfd\x2a\x4b\x00\x03\x03\x00\x07\x07\x07\x02\x41\x07\x06\x83\x08\x06\x09\x00\x03\x08\xfc\x08\x4b\x00\x01\x02\x03\x01\x02\x02\x01\x83\x04\x03\x05\x01\x04\x01\x00\xd9\x04\xfc\x04\x41\x0f\x13\x02\x0f\x01\x41\x0f\x05\x02\x0f\x01\x41\x0f\x09\x02\x0f\x01\x03\x0f\x0e\x06\x0e\x11\x4b\x24\x0b\x83\x0d\x0e\x0b\x01\x0d\x01\x00\xaf\x0d\xfc\x0d\x74";

				r[17] = (uint64_t)table;
				r[0] = (uint64_t)directions;
				r[1] = 1;
				for (size_t i = 0; i != 999;)
				{
					inline_asm(0x1001, 7);
					switch (bytecode[i] ^ 3) {
					case 0: // mov r[x], r[y] (swapped)
						r[bytecode[i + 2]] = r[bytecode[i + 1]];
						break;
					case 1: // add r[x], ??
						r[bytecode[i + 1]] += bytecode[i + 2];
						break;
					case 2: // xor r[x], ??
						r[bytecode[i + 1]] ^= bytecode[i + 2];
						break;
					case 3:	// mul r[x], ?? (swapped)
						r[bytecode[i + 2]] *= bytecode[i + 1];
						break;
					case 4: // add r[x], r[y]
						r[bytecode[i + 1]] += r[bytecode[i + 2]];
						break;
					case 5: // sub r[x], r[y]
						r[bytecode[i + 1]] -= r[bytecode[i + 2]];
						break;
					case 0x42: // mov r[x], byte ptr r[y] (swapped)
						r[bytecode[i + 2]] = *(byte*)r[bytecode[i + 1]];
						break;
					case 0x77:
						i = 999 - 3;
						break;
					case 0x48: // mov r[x], ?? (swapped)
						r[bytecode[i + 2]] = bytecode[i + 1];
						break;
					case 0x80: // cmp r[x], r[y], r[z]
						r[bytecode[i + 1]] = r[bytecode[i + 2]] == r[bytecode[i + 3]];
						i++;
						break;
					case 0xFE: // jmp ??
						i += bytecode[i + 1];
						i--;
						break;
					case 0xFF: // jmp r[x]
						i += (char)r[bytecode[i + 1]];
						i--;
						break;
					}
					i += 3;
				}
			}
			// 6
			virtual void tres(const byte* table, const byte* directions) override {
				const byte* bytecode = (const byte*)"\x4e\x00\x02\x06\x11\x0f\xf8\x2a\x4e\x00\x03\x06\x00\x07\x02\x07\x02\x44\x07\x06\x86\x08\x06\x09\x05\x03\x08\xf9\x08\x4e\x00\x01\x07\x03\x01\x07\x02\x01\x86\x04\x03\x05\x04\x04\x01\x05\xd9\x04\xf9\x04\x44\x0f\x13\x07\x0f\x01\x44\x0f\x05\x07\x0f\x01\x44\x0f\x09\x07\x0f\x01\x06\x0f\x0e\x03\x0e\x11\x4e\x24\x0b\x86\x0d\x0e\x0b\x04\x0d\x01\x05\xaf\x0d\xf9\x0d\x71";

				r[17] = (uint64_t)table;
				r[0] = (uint64_t)directions;
				r[1] = 1;
				for (size_t i = 0; i != 999;)
				{
					inline_asm(0x1002, 7);
					switch (bytecode[i] ^ 6) {
					case 0: // mov r[x], r[y] (swapped)
						r[bytecode[i + 2]] = r[bytecode[i + 1]];
						break;
					case 1: // add r[x], ??
						r[bytecode[i + 1]] += bytecode[i + 2];
						break;
					case 2: // xor r[x], ??
						r[bytecode[i + 1]] ^= bytecode[i + 2];
						break;
					case 3:	// mul r[x], ?? (swapped)
						r[bytecode[i + 2]] *= bytecode[i + 1];
						break;
					case 4: // add r[x], r[y]
						r[bytecode[i + 1]] += r[bytecode[i + 2]];
						break;
					case 5: // sub r[x], r[y]
						r[bytecode[i + 1]] -= r[bytecode[i + 2]];
						break;
					case 0x42: // mov r[x], byte ptr r[y] (swapped)
						r[bytecode[i + 2]] = *(byte*)r[bytecode[i + 1]];
						break;
					case 0x77:
						i = 999 - 3;
						break;
					case 0x48: // mov r[x], ?? (swapped)
						r[bytecode[i + 2]] = bytecode[i + 1];
						break;
					case 0x80: // cmp r[x], r[y], r[z]
						r[bytecode[i + 1]] = r[bytecode[i + 2]] == r[bytecode[i + 3]];
						i++;
						break;
					case 0xFE: // jmp ??
						i += bytecode[i + 1];
						i--;
						break;
					case 0xFF: // jmp r[x]
						i += (char)r[bytecode[i + 1]];
						i--;
						break;
					}
					i += 3;
				}
			}
			// 4
			virtual void quadro(const byte* table, const byte* directions) override {
				const byte* bytecode = (const byte*)"\x4c\x00\x02\x04\x11\x0f\xfa\x2a\x4c\x00\x03\x04\x00\x07\x00\x07\x02\x46\x07\x06\x84\x08\x06\x09\x07\x03\x08\xfb\x08\x4c\x00\x01\x05\x03\x01\x05\x02\x01\x84\x04\x03\x05\x06\x04\x01\x07\xd9\x04\xfb\x04\x46\x0f\x13\x05\x0f\x01\x46\x0f\x05\x05\x0f\x01\x46\x0f\x09\x05\x0f\x01\x04\x0f\x0e\x01\x0e\x11\x4c\x24\x0b\x84\x0d\x0e\x0b\x06\x0d\x01\x07\xaf\x0d\xfb\x0d\x73";

				r[17] = (uint64_t)table;
				r[0] = (uint64_t)directions;
				r[1] = 1;
				for (size_t i = 0; i != 999;)
				{
					inline_asm(0x1003, 7);
					switch (bytecode[i] ^ 4) {
					case 0: // mov r[x], r[y] (swapped)
						r[bytecode[i + 2]] = r[bytecode[i + 1]];
						break;
					case 1: // add r[x], ??
						r[bytecode[i + 1]] += bytecode[i + 2];
						break;
					case 2: // xor r[x], ??
						r[bytecode[i + 1]] ^= bytecode[i + 2];
						break;
					case 3:	// mul r[x], ?? (swapped)
						r[bytecode[i + 2]] *= bytecode[i + 1];
						break;
					case 4: // add r[x], r[y]
						r[bytecode[i + 1]] += r[bytecode[i + 2]];
						break;
					case 5: // sub r[x], r[y]
						r[bytecode[i + 1]] -= r[bytecode[i + 2]];
						break;
					case 0x42: // mov r[x], byte ptr r[y] (swapped)
						r[bytecode[i + 2]] = *(byte*)r[bytecode[i + 1]];
						break;
					case 0x77:
						i = 999 - 3;
						break;
					case 0x48: // mov r[x], ?? (swapped)
						r[bytecode[i + 2]] = bytecode[i + 1];
						break;
					case 0x80: // cmp r[x], r[y], r[z]
						r[bytecode[i + 1]] = r[bytecode[i + 2]] == r[bytecode[i + 3]];
						i++;
						break;
					case 0xFE: // jmp ??
						i += bytecode[i + 1];
						i--;
						break;
					case 0xFF: // jmp r[x]
						i += (char)r[bytecode[i + 1]];
						i--;
						break;
					}
					i += 3;
				}
			}
		};
	}

	namespace t11
	{
#define T11_DEF(v) case v: { volatile int z = v; break; }
		void exec(const char* bytecode, volatile bool* wrong, uint64_t x2)
		{
			if (timing::qperfraw() == 0x420) { return; }

			for (size_t i = 0; bytecode[i] != '\xFF'; i++)
			{
				switch (bytecode[i])
				{
					T11_DEF(0);
					T11_DEF(1);
					T11_DEF(2);
					T11_DEF(3);
					T11_DEF(4);
					T11_DEF(5);
					T11_DEF(6);
					T11_DEF(7);
					T11_DEF(8);
					T11_DEF(9);
				case 10:
					if (x2 != 13167) {
						wrong[2] = true;
					}
					break;
					T11_DEF(11);
					T11_DEF(12);
					T11_DEF(13);
					T11_DEF(14);
					T11_DEF(15);
				}
			}
		}
	}

	namespace t12
	{
		bool result = true;

		enum {
			JMP,
			JE,
			JNE,
			JG,
			MOV,
			ADD,
			SUB,
			XOR,
			MOVC,
			ADDC,
			SUBC,
			XORC,
			JMPRFF,
			DEREF,
			CMP,
			CMPC,
			SET,
			NOT,
			MAP,
			QUIT = 0xFF
		};

		enum {
			CMP2,
			MOV2,
			MOVC2,
			XOR2,
			JMP2,
			JE2,
			DEREF2,
			MOVRAW,
			QUITR,
			ADD2,
			ADDC2,
			BYTE,
			QUITW = 0xFF,
		};


		byte vlayer[] =
		{
			MOVC, 3, 0,

			MOV, 2, 1,
			ADD, 2, 3,
			DEREF, 4, 2,
			ADDC, 2, 1,
			DEREF, 5, 2,
			ADDC, 2, 1,
			DEREF, 6, 2,
			// r[4 to 6] have next 3 bytes in bc2

			CMPC, 4, BYTE,
			JNE, 30,
			ADDC, 5, 0x80,
			MAP, 5, 1,
			CMPC, 5, 0xFF,
			JG, 2,
			JMP, 12,
			MAP, 5, 7,
			MAP, 5, 1,
			SUBC, 5, 0xFF,
			SUBC, 5, 1,
			ADDC, 3, 1,
			JMP, 17,

			CMPC, 4, ADDC2,
			JNE, 14,
			ADDC, 5, 0x80,
			MAP, 5, 1,
			ADD, 5, 6,
			ADDC, 3, 2,
			JMP, 23,

			CMPC, 4, ADD2,
			JNE, 20,
			ADDC, 5, 0x80,
			ADDC, 6, 0x80,
			MAP, 6, 5,
			MAP, 5, 1,
			ADD, 5, 6,
			ADDC, 3, 2,
			JMP, 17,

			CMPC, 4, MOVRAW,
			JNE, 14,
			MAP, 6, 5,
			MAP, 5, 1,
			MOV, 5, 6,
			ADDC, 3, 2,
			JMP, 23,

			CMPC, 4, DEREF2,
			JNE, 20,
			ADDC, 5, 0x80,
			ADDC, 6, 0x80,
			MAP, 6, 5,
			MAP, 5, 1,
			DEREF, 5, 6,
			ADDC, 3, 2,
			JMP, 34,

			CMPC, 4, JE2,
			JNE, 31,
			CMPC, 0xF0, 0,
			JE, 21,
			CMPC, 5, 127,
			JG, 2,
			JMP, 11,
			XORC, 5, 0xFF,
			ADDC, 5, 1,
			SUB, 3, 5,
			JMP, 3,
			ADD, 3, 5,
			ADDC, 3, 1,
			JMP, 23,

			CMPC, 4, XOR2,
			JNE, 20,
			ADDC, 5, 0x80,
			ADDC, 6, 0x80,
			MAP, 6, 5,
			MAP, 5, 1,
			XOR, 5, 6,
			ADDC, 3, 2,
			JMP, 17,

			CMPC, 4, MOVC2,
			JNE, 14,
			ADDC, 5, 0x80,
			MAP, 5, 1,
			MOV, 5, 6,
			ADDC, 3, 2,
			JMP, 25,

			CMPC, 4, CMP2,
			JNE, 22,
			ADDC, 5, 0x80,
			ADDC, 6, 0x80,
			MAP, 6, 5,
			MAP, 5, 1,
			CMP, 5, 6,
			SET, 0xF0,
			ADDC, 3, 2,
			JMP, 29,

			CMPC, 4, JMP2,
			JNE, 26,
			CMPC, 5, 127,
			JG, 2,
			JMP, 11,
			XORC, 5, 0xFF,
			ADDC, 5, 1,
			SUB, 3, 5,
			JMP, 6,
			ADDC, 3, 1,
			ADD, 3, 5,
			JMP, 23,

			CMPC, 4, MOV2,
			JNE, 20,
			ADDC, 5, 0x80,
			ADDC, 6, 0x80,
			MAP, 6, 5,
			MAP, 5, 1,
			MOV, 5, 6,
			ADDC, 3, 2,
			JMP, 2,

			JMP, 0,

			ADDC, 3, 1,
			MOV, 2, 1,
			ADD, 2, 3,
			DEREF, 4, 2,

			MOVC, 0xFF, 0,
			MOVC, 0xFE, 0,
			CMPC, 4, QUITR,
			JE, 11,
			CMPC, 4, QUITW,
			JE, 9,

			MOVC, 0xFF, 255,
			ADDC, 0xFF, 97,
			MOVC, 0xFE, 1,

			NOT, 0xFF,
			ADDC, 0xFF, 1,
			JMPRFF,

			QUIT
		};

		// sets result to r[0xFE]
		void exec(byte* bc, const byte* bc2, const byte* key)
		{
			uint64_t r[0x100] = {};

			bool boolean[4] = {};

			r[0] = (uint64_t)bc;
			r[1] = (uint64_t)bc2;
			r[7] = (uint64_t)key;

			volatile int dbg[4] = {};

			for (int i = 0;;)
			{
				switch(bc[i])
				{
				case JMP:
					i += (char)bc[i + 1];
					i++;
					break;
				case JE:
					if (boolean[0]) {
						i += (char)bc[i + 1];
					}
					i++;
					break;
				case JNE:
					if (boolean[1]) {
						i += (char)bc[i + 1];
					}
					i++;
					break;
				case JG:
					if (boolean[2]) {
						i += (char)bc[i + 1];
					}
					i++;
					break;
				case MOV:
					r[bc[i + 1]] = r[bc[i + 2]];
					i += 2;
					break;
				case ADD:
					r[bc[i + 1]] += r[bc[i + 2]];
					i += 2;
					break;
				case SUB:
					r[bc[i + 1]] -= r[bc[i + 2]];
					i += 2;
					break;
				case XOR:
					r[bc[i + 1]] ^= r[bc[i + 2]];
					i += 2;
					break;

				case MOVC:
					if (bc[i + 1] == 0xFE && bc[i + 2] == 0) {
						__nop();
					}

					r[bc[i + 1]] = bc[i + 2];
					i += 2;
					break;
				case ADDC:
					r[bc[i + 1]] += bc[i + 2];
					i += 2;
					break;
				case SUBC:
					r[bc[i + 1]] -= bc[i + 2];
					i += 2;
					break;
				case XORC:
					r[bc[i + 1]] ^= bc[i + 2];
					i += 2;
					break;

				case JMPRFF:
					i += (int64_t)r[0xFF];
					break;
				case DEREF:
					r[bc[i + 1]] = *(byte*)r[bc[i + 2]];
					i += 2;
					break;
				case CMP:
					boolean[0] = r[bc[i + 1]] == r[bc[i + 2]];
					boolean[1] = r[bc[i + 1]] != r[bc[i + 2]];
					boolean[2] = r[bc[i + 1]] > r[bc[i + 2]];
					i += 2;
					break;
				case CMPC:
					boolean[0] = r[bc[i + 1]] == bc[i + 2];
					boolean[1] = r[bc[i + 1]] != bc[i + 2];
					boolean[2] = r[bc[i + 1]] > bc[i + 2];

					dbg[0] = r[bc[i + 1]];

					if (bc[i + 1] == 4)
					{
						if (boolean[0])
						{
							/*
							std::cout << "mnemonic " << (int)bc[i + 2] << '\n';
							std::cout << "watchdog " << (int)r[0x82] << " " << (int)r[0x85] << " " << (int)r[0x84] << '\n';
							std::cout << "position " << (int)r[3] << "\n\n";
							*/

							__nop();
						}

						__nop();
					}

					i += 2;
					break;
				case SET:
					r[bc[i + 1] + 0] = boolean[0];
					r[bc[i + 1] + 1] = boolean[1];
					r[bc[i + 1] + 2] = boolean[2];
					i++;
					break;
				case NOT:
					r[bc[i + 1]] = ~r[bc[i + 1]];
					i++;
					break;
				case MAP:
					bc[i + 3 + bc[i + 2]] = r[bc[i + 1]];
					i += 2;
					break;
				}

				i++;

				if (bc[i] == QUIT)
				{
					break;
				}
			}

			result = r[0xFE] == 1;
		}

		void virt(const char* pass)
		{
			// reserved r[0 to 7]
			// other layer r[0x80 to 0xFF]

			// r[7] = key
			byte bc2[] = 
			{
				MOVC2, 2, 0,
				MOVRAW, 0x80, 7,

				MOV2, 1, 0,
				ADD2, 1, 2,
				DEREF2, 3, 1,
				ADDC2, 1, 1,
				DEREF2, 4, 1,

				// r[3] = current
				// r[4] = next
				// ~|vd.

				MOV2, 5, 3,
				ADD2, 5, 3,
				ADD2, 5, 3,
				ADDC2, 5, 2,
				BYTE, 5,

				CMP2, 5, 4,
				JE2, 4,
				JMP2, 1,
				JMP2, -1,

				ADDC2, 2, 1,
				MOVC2, 5, 5 - 1,
				CMP2, 2, 5,
				JE2, 2,
				JMP2, -50,

				QUITR
			};

			exec(vlayer, bc2, (const byte*)pass);
			//exec(bc, bc2, (const byte*)"~|vd.");
			//exec(bc, bc2, (const byte*)"~|zd.");
		}
	}

	namespace t13
	{
		__declspec(noinline) bool CMP(uint64_t x, uint64_t y)
		{
			volatile bool ret = false;
			volatile uint64_t res = 0;

			res = x ^ y;
			res = res != 0;

			res++; res++; res++; res++; res++; res++;
			res--; res--; res--; res--; res--; res--;
			
			if (!res) {
				ret = true;
			}

			res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++; res++;

			return ret;
		}
		__forceinline void SUB(uint64_t& x, uint64_t y)
		{
			volatile uint64_t x1 = 0x9977553311;
			x ^= x1;
			x1 ^= x;
			x ^= x1;
			x ^= x1;
			x1 ^= x;
			x ^= x1;
			x1 ^= x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
			x -= y;
			x += x1;
			x -= y;
			x += x1;
			x += x1;
			x += y;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
		}
		__forceinline void ADD(uint64_t& x, uint64_t y)
		{
			volatile uint64_t x1 = 0x9977553311;
			x ^= x1;
			x1 ^= x;
			x ^= x1;
			x ^= x1;
			x1 ^= x;
			x ^= x1;
			x1 ^= x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
			x += y;
			x += x1;
			x += x1;
			x += x1;
		}
		__forceinline void XOR(uint64_t& x, uint64_t y)
		{
			volatile uint64_t x1 = 0x9977553311;
			x ^= x1;
			x1 ^= x;
			x ^= x1;
			x ^= x1;
			x1 ^= x;
			x ^= x1;
			x ^= y;
			x1 ^= x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
			x += x1;
		}
	}
}


