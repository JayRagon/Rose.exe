#pragma once

// toolkit for handling memory-related tasks

#include <iostream>
#include <Windows.h>

#include "cout_color.h"
#include "sigs.hpp"

HANDLE phandle = GetCurrentProcess();

#define xormem_f(addr, size, key) \
xormem(addr, size, (const char*)key, sizeof key)

#define xorsig_f(pattern, mask, pattern2, mask2, key, startaddr, index) \
xorsig(pattern, sizeof pattern, mask, key, sizeof key, pattern2, sizeof pattern2, mask2, startaddr, index)

#define deref8 *(unsigned char*)
#define deref16 *(WORD*)
#define deref32 *(DWORD*)
#define deref64 *(uintptr_t*)


char* i2c(int32_t x)
{
	char* y = new char[4];
	for (size_t i = 0; i < 4; i++)
	{
		y[i] = (x & (0xFF << (i * 8))) >> (i * 8);
	}
	return y;
}

// make sure to subtract null terminator
__declspec(noinline) uintptr_t internal_pattern_scan(uintptr_t startaddr, const char* pattern, const char* mask,
	uint32_t aoblen, uint32_t index, uintptr_t scan_len)
{
	uint32_t found_count = 0;
	for (uintptr_t i = startaddr; i < startaddr + scan_len; i++)
	{
		bool found = true;
		for (size_t x = 0; x < aoblen; x++)
		{
			if (pattern[x] != *(char*)(i + x) && mask[x] != '?') {
				found = false;
				break;
			}
		}

		if (found) {
			found_count++;
			if (found_count == index) {
				return i;
			}
		}
	}
	return 0;
}

bool mem_check(const char* x, const char* mask, int size, uintptr_t offset)
{
	char* buffer = new char[size];

	memcpy(buffer, (const void*)offset, size);

	bool matching = true;
	for (size_t i = 0; i < size; i++)
	{
		if (buffer[i] != x[i] && mask[i] != '?') {
			matching = false;
			break;
		}
	}

	delete[] buffer;
	return matching;
}

// read the .rdata address to which the instruction of the __sig is pointing to
uintptr_t read_ptr(uintptr_t sig_offset)
{
	char bytes[12] = {};
	memcpy(bytes, (const void*)sig_offset, 12);

	int rel_ptr = *(int*)(bytes + 3);

	uintptr_t ptr = rel_ptr + sig_offset + 7;

	return ptr;
}

uintptr_t read_call(uintptr_t sig_offset)
{
	char bytes[12] = {};
	memcpy(bytes, (const void*)sig_offset, 12);

	int rel_ptr = *(int*)(bytes + 8);

	uintptr_t ptr = rel_ptr + sig_offset + 12;

	return ptr;
}

__declspec(noinline) uintptr_t find_signature(uintptr_t startaddr,
	uint32_t signature_id, uint32_t index, uintptr_t scan_len = 0x10000000)
{
	uintptr_t ret_addr = 0;

	char* id_c = i2c(signature_id);

	int current_index = 1;
	for (int i = 1; i < 1000; i++)
	{
		uintptr_t offset = internal_pattern_scan(startaddr, sig_pattern, sig_mask, sig_size, i, scan_len);
		if (offset == 0) { break; }

		uintptr_t id_ptr = read_ptr(offset);
		uintptr_t func_ptr = read_call(offset);

		if (id_ptr > startaddr + 0x1000000 || func_ptr > startaddr + 0x1000000 ||
			id_ptr < startaddr - 0x1000000 || func_ptr < startaddr - 0x1000000) { continue; }

		const char func_pattern[] = "\x48\x89\x4C\x24\x08\x48\x83\xEC\x18\x48\x8B\x44\x24\x20\x8B\x00\x89\x04\x24\x90\x90\x48\x83\xC4\x18\xC3";
		const char func_mask[] = "xxxxxxxxxxxxxxxxxxxxxxxxxx";

		if (!mem_check((const char*)id_c, "xxxx", 4, id_ptr)) { continue; }
		if (!mem_check(func_pattern, func_mask, 27 - 1, func_ptr)) { continue; }

		if (current_index == index) {
			ret_addr = offset;
			break;
		}
		else {
			current_index++;
		}
	}

	delete[] id_c;
	return ret_addr;
}

void patchmem(uintptr_t addr, const char* bytes, uint32_t size)
{
	DWORD old;
	VirtualProtectEx(phandle, (LPVOID)addr, size, PAGE_EXECUTE_READWRITE, &old);

	memcpy((void*)addr, bytes, size);

	DWORD dummy0;
	VirtualProtectEx(phandle, (LPVOID)addr, size, old, &dummy0);
}

__declspec(noinline) void xormem(uintptr_t addr, uint32_t size, const char* key, uint32_t keylen)
{
	DWORD old;
	VirtualProtectEx(phandle, (LPVOID)addr, size, PAGE_EXECUTE_READWRITE, &old);

	for (size_t i = 0; i < size; i++)
	{
		deref8(addr + i) ^= key[i % keylen];
	}

	DWORD dummy0;
	VirtualProtectEx(phandle, (LPVOID)addr, size, old, &dummy0);
}

__declspec(noinline) void xorpattern(const char* pattern, uint32_t psize,
	const char* mask,
	const char* key, uint32_t keysize,
	uintptr_t startaddr, uint32_t index)
{
	uintptr_t addr1 = internal_pattern_scan(startaddr, pattern, mask, psize, index, 0x10000) + 80;
	uintptr_t addr2 = internal_pattern_scan(startaddr, pattern, mask, psize, index + 1, 0x10000);

	uint32_t delta = addr2 - addr1;

	if (addr1 != 0 && addr2 != 0) {
		xormem(addr1, delta, key, keysize);
	}
	else
	{
		print("You modified or set a breakpoint at the wrong part of the code.|\nThis code is highly important to keep the program running smoothly.|\nReopen the program and everything will be fine.|\n");
	}
}


__declspec(noinline) void xor_signature(const char* key, uint32_t keysize,
	uint32_t signature_id, uintptr_t func)
{
	uintptr_t addr1 = find_signature(func, signature_id, 1) + sig_size;
	uintptr_t addr2 = find_signature(func, signature_id, 2);

	volatile uint32_t delta = addr2 - addr1;

	if (addr1 != 0 && addr2 != 0) {
		xormem(addr1, delta, key, keysize);
	}
	else
	{
		set_cout_color(COUT_GREEN);
		print("You modified or set a breakpoint at the wrong part of the code.|\nThis code is important to keep the program running.|\nReopen the program.|\n");
		Sleep(10000000);
	}
}

