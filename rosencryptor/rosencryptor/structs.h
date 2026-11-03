#pragma once
#include <cstdint>

typedef struct _keystruct
{
	const char* key;
	uint32_t keysize;
} keystruct;

typedef struct _asmstruct
{
	const char* bytes;
	uint32_t size;
} asmstruct;



typedef struct _strc
{
	const char* str;
	uint32_t key_index;
} strc;