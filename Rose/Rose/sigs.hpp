#pragma once

#include <iostream>
#include <Windows.h>

#include <intrin.h>

#include "mem.h"

#define sigc(pattern, index) \
sigcv::sigcv<sizeof pattern>(pattern, index)

// .rdata ID
// this abuses volatile to forcefully store the index in a very structured manner
// it also abuses pragma optimize("", off)
#define __sig(index) \
if (0 == 0) { volatile static const uint32_t x = index; __adv(&x); }

// in case opti is doing something random
// only use this as a last resort
// this basically just is the shotgun approach, one of these sigs shouldn't be malformed by an optimization
#define __sig_safe(index) \
__sig(index); __sig(index); __sig(index); __sig(index); __sig(index); __sig(index); __sig(index)


const char* sig_pattern = "\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00";
const char* sig_mask = "xxx????x????";
const uint32_t sig_size = 13 - 1;

// keysize without null terminator
// i think the __nop() is for when placing breakpoints there is a place to put it before the __sig and after the __xorsig
// so that the next stage can decrypt but i'm not 100% sure on that. wrote this too long ago and didn't add a comment
#define __xorsig(func, key, keysize, index) \
xor_signature(key, keysize, index, (uintptr_t)func); \
__nop()


/*
//ye olde __xorsig(func, key, keysize, index)
xorpattern(sigc("\xB9\x07\x00\x00\x00\xE8\x00\xFF\xFF\xFF\xB9\x07\x00\x00\x00\xE8\x00\xFF\xFF\xFF\xB9\x07\x00\x00\x00\xE8\x00\xFF\xFF\xFF\xB9\x07\x00\x00\x00\xE8\x00\xFF\xFF\xFF\xB9\x04\x00\x00\x00\xE8\x00\xFF\xFF\xFF\xB9\x04\x00\x00\x00\xE8\x00\xFF\xFF\xFF\xB9\x04\x00\x00\x00\xE8\x00\xFF\xFF\xFF\xB9\x04\x00\x00\x00", index).p, \
76 - 1, "xxxxxx????xxxxxx????xxxxxx????xxxxxx????xxxxxx????xxxxxx????xxxxxx????xxxxx", key, keysize, (uintptr_t)&(func), 1)
*/


// deprecated, don't use with global optimizations please
#pragma optimize("", off)
void __sigmaker(int x)
{
	__nop();
	return;
}
#pragma optimize("", on)

// safe to use with global optmization
#pragma optimize("", off)
void __adv(volatile const uint32_t* index)
{
	volatile uint32_t x = *index;
	__nop();
	__nop();
	return;
}
#pragma optimize("", on)


namespace sigcv
{
	template <uint32_t pattern_size>
	class sigcv
	{
	public:
		__declspec(noinline) sigcv(const char* pattern, int index)
		{
			for (size_t i = 0; i < pattern_size; i++)
			{
				p[i] = pattern[i];
			}
			for (size_t i = 1; i < pattern_size; i += 10)
			{
				p[i] = index & 0xFF;
				if (index > 0xFF) {
					p[i + 1] = index >> 8;
				}
			}
		}

		char p[pattern_size] = {};
	};
}

