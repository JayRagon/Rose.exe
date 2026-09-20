#pragma once
#include <Windows.h>
#include <iostream>
#include "file.h"
#include <vector>

// all functions from file.h but in memory for speed and use with zydis

typedef struct _file_struct
{
	std::vector<BYTE> bytes;
	PIMAGE_SECTION_HEADER text = nullptr, data = nullptr, rdata = nullptr, pdata = nullptr;
	IMAGE_SECTION_HEADER** sections = &text; // sections is indexable
	uint64_t base_addr;
	uint32_t section_count = 4;
} file_struct;


void m_patch(file_struct* file, const char* bytes, uint32_t offset, uint32_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		file->bytes[i + offset] = bytes[i];
	}
}

void m_read(file_struct* file, BYTE* buffer, uint32_t offset, uint32_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		buffer[i] = file->bytes[i + offset];
	}
}


bool m_match(file_struct* file, const char* x, const char* mask, int size, uint32_t offset)
{
	bool matching = true;
	for (size_t i = 0; i < size; i++)
	{
		if ((char)file->bytes[i + offset] != x[i] && mask[i] != '?') {
			matching = false;
			break;
		}
	}

	return matching;
}

uintptr_t m_read_ptr(file_struct* file, uintptr_t sig_offset)
{
	int rel_ptr = *reinterpret_cast<const int*>(&file->bytes[sig_offset + 3]);
	
	uintptr_t ptr = rel_ptr + sig_offset + 7;

	return ptr;
}

uintptr_t m_read_call(file_struct* file, uintptr_t sig_offset)
{
	int rel_ptr = *reinterpret_cast<const int*>(&file->bytes[sig_offset + 8]);

	uintptr_t ptr = rel_ptr + sig_offset + 12;

	return ptr;
}

size_t m_pattern_scan(file_struct* file, const char* sig, const char* mask, int length, int index)
{
	int currentindex = 1;
	uintptr_t offset = 0;
	for (int32_t i = 0; i < file->bytes.size() - length; i++)
	{
		for (int32_t u = 0; u < length; u++)
		{
			if (sig[u] != (char)file->bytes[u + i] && mask[u] == 'x')
			{
				break;
			}

			if (u == length - 1)
			{
				if (currentindex < index)
				{
					currentindex++;
					break;
				}

				return i;
			}
		}
	}

	return 0;
}

void m_str_crypt(file_struct* file, const char* str, keystruct key)
{
	uint32_t key_len = key.keysize;
	uint32_t p_len = strlen(str);
	char* mask = new char[p_len];
	for (size_t i = 0; i < p_len; i++)
	{
		mask[i] = 'x';
	}

	uint32_t offset = m_pattern_scan(file, str, mask, p_len, 1);
	delete[] mask;

	if (offset == 0) { 
		std::cerr << "Make sure you got your str_crypt strings right!\n"; 
	}

	uint32_t str_len = 0;
	for (uint32_t u = 0; u < file->bytes.size() - offset; u++)
	{
		if (file->bytes[u + offset] == '\x00') {
			str_len = u;
			break;
		}
	}

	for (size_t i = 0; i < str_len; i++)
	{
		file->bytes[i + offset] ^= key.key[i % key_len];
	}
}

uintptr_t m_find_signature(file_struct* file, const char* sig, const char* mask, int length, int index,
	uint32_t signature_id)
{
	char* id_c = i2c(signature_id);

	uintptr_t return_offset = 0;

	int current_index = 1;
	for (int i = 1; i < 100000; i++)
	{
		uintptr_t offset = m_pattern_scan(file, sig, mask, length, i);
		if (offset == 0) { break; }

		uintptr_t id_ptr = m_read_ptr(file, offset);
		uintptr_t func_ptr = m_read_call(file, offset);

		const char func_pattern[] = "\x48\x89\x4C\x24\x08\x48\x83\xEC\x18\x48\x8B\x44\x24\x20\x8B\x00\x89\x04\x24\x90\x90\x48\x83\xC4\x18\xC3";
		const char func_mask[] = "xxxxxxxxxxxxxxxxxxxxxxxxxx";


		// mem offset of int
		id_ptr = (id_ptr - file->text->PointerToRawData) + file->text->VirtualAddress;

		// file offset
		id_ptr = (id_ptr + file->rdata->PointerToRawData) - file->rdata->VirtualAddress;

		if (id_ptr > file->bytes.size() || func_ptr > file->bytes.size()) { continue; }

		if (!m_match(file, (const char*)id_c, "xxxx", 4, id_ptr)) { continue; }
		if (!m_match(file, func_pattern, func_mask, 27 - 1, func_ptr)) { continue; }

		if (current_index == index) {
			return_offset = offset;
			break;
		}
		else {
			current_index++;
		}
	}

	delete[] id_c;
	return return_offset;
}

