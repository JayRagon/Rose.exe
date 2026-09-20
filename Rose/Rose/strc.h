#pragma once
#include <Windows.h>
#include <iostream>

// string crypt
#define strc(str, key, key_len) \
__strc(str, sizeof str - 1, key, key_len)

char* __strc(const char* str, uint32_t str_len, const char* key, uint32_t key_len)
{
	char* dec = new char[str_len + 1];

	for (size_t i = 0; i < str_len; i++)
	{
		dec[i] = str[i] ^ key[i % key_len];
	}

	dec[str_len] = 0;
	return dec;
}
