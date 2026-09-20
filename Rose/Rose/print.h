#pragma once
#include <iostream>
#include <Windows.h>
#include "strc.h"

#pragma optimize("", off)
void print(const char* x, int ms = 40)
{
	int bait = 0x3040;

	for (size_t i = 0; x[i] != NULL; i++)
	{
		if (x[i] == '|') {
			Sleep(300);
		}
		else {
			std::cout << x[i];
			Sleep(ms);
		}
	}

	const char* hint = "Hint: A bar looks like | and has hex code of 7C";
}
#pragma optimize("", on)

#define print_c(str, key, klen, sleep) \
__print_c(str, sizeof str - 1, key, klen, sleep)

#define cout_c(str, key, klen) \
__cout_c(str, sizeof str - 1, key, klen)

__declspec(noinline) void __print_c(const char* str, uint32_t str_len, const char* key, uint32_t key_len, int sleep = 40)
{
	char* dec = __strc(str, str_len, key, key_len);
	print((const char*)dec, sleep);
	delete[] dec;
}

__declspec(noinline) void __cout_c(const char* str, uint32_t str_len, const char* key, uint32_t key_len)
{
	char* dec = __strc(str, str_len, key, key_len);
	std::cout << dec;
	delete[] dec;
}

