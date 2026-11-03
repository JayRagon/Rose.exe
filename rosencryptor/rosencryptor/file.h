#pragma once
#include <fstream>
#include <filesystem>
#include <Windows.h>

#include "structs.h"

PIMAGE_SECTION_HEADER find_section(const char* filepath, const char* section)
{
	// open it and get the size
	HANDLE hfile = CreateFileA(filepath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	DWORD file_size = GetFileSize(hfile, 0);

	DWORD bytes_read;

	// load it into memory
	LPBYTE file_buffer = (LPBYTE)malloc(file_size);
	if (file_buffer == NULL) { return nullptr; }
	if (!ReadFile(hfile, file_buffer, file_size, &bytes_read, 0)) {
		return nullptr;
	}

	PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)file_buffer;

	// basic checks
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
		CloseHandle(hfile);
		return nullptr;
	}

	PIMAGE_NT_HEADERS nt_header = (PIMAGE_NT_HEADERS)(file_buffer + dos_header->e_lfanew);

	if (nt_header->Signature != IMAGE_NT_SIGNATURE) {
		CloseHandle(hfile);
		return nullptr;
	}

	PIMAGE_SECTION_HEADER section_header = (PIMAGE_SECTION_HEADER)IMAGE_FIRST_SECTION(nt_header);

	for (size_t i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
	{
		//std::cout << "section " << section_header->Name << "\naddr 0x" << std::hex << section_header->VirtualAddress << std::dec << '\n';
		if (strcmp((const char*)section_header->Name, section) == 0) {
			CloseHandle(hfile);
			return section_header;
		}
		section_header++;
	}

	std::cout << section << " not found\n";
	CloseHandle(hfile);
	return nullptr;
}

uint64_t get_base_addr(const char* filepath)
{
	// open it and get the size
	HANDLE hfile = CreateFileA(filepath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	DWORD file_size = GetFileSize(hfile, 0);

	DWORD bytes_read;

	// load it into memory
	LPBYTE file_buffer = (LPBYTE)malloc(file_size);
	if (file_buffer == NULL) { return 0; }
	if (!ReadFile(hfile, file_buffer, file_size, &bytes_read, 0)) {
		return 0;
	}

	PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)file_buffer;

	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
		CloseHandle(hfile);
		return 0;
	}

	PIMAGE_NT_HEADERS nt_header = (PIMAGE_NT_HEADERS)(file_buffer + dos_header->e_lfanew);

	if (nt_header->Signature != IMAGE_NT_SIGNATURE) {
		CloseHandle(hfile);
		return 0;
	}

	CloseHandle(hfile);
	return nt_header->OptionalHeader.ImageBase;
}

uint32_t get_file_size(std::fstream* fs)
{
	uint32_t file_start = fs->tellg();
	fs->seekg(0, std::ios::end);
	uint32_t file_end = fs->tellg();
	uint32_t file_size = file_end - file_start;

	fs->seekg(0, std::ios::beg);

	return file_size;
}

void patch_bytes(std::fstream* fs, int offset, const char* bytes, int len)
{
	fs->seekg(offset, std::ios::beg);
	fs->write(bytes, len);
}

void read_bytes(std::fstream* fs, char* buffer, uintptr_t offset, size_t length)
{
	fs->seekg(offset, std::ios::beg);

	buffer[length] = '\0';
	fs->read(buffer, length);
}

void cout_bytes(std::fstream* fs, int offset, int length)
{
	fs->seekg(offset, std::ios::beg);

	char* buffer = new char[length + 1];

	buffer[length] = '\0';
	fs->read(buffer, length);
	std::cout << std::hex << (int)buffer << std::dec << '\n';

	delete[] buffer;
}

void endian32(char* x)
{
	char y[4] = { x[0], x[1], x[2], x[3] };

	for (size_t i = 0; i < 4; i++)
	{
		x[i] = y[3 - i];
	}
}

char* i2c(int32_t x)
{
	char* y = new char[4];
	for (size_t i = 0; i < 4; i++)
	{
		y[i] = (x & (0xFF << (i * 8))) >> (i * 8);
	}
	return y;
}

bool match(std::fstream* fs, const char* x, const char* mask, int size, uint32_t offset)
{
	char* buffer = new char[size + 1];
	read_bytes(fs, buffer, offset, size);

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

uintptr_t read_ptr(std::fstream* fs, uintptr_t sig_offset)
{
	char bytes[12 + 1] = {};
	read_bytes(fs, bytes, sig_offset, 12);

	int rel_ptr = *(int*)(bytes + 3);

	uintptr_t ptr = rel_ptr + sig_offset + 7;

	return ptr;
}

uintptr_t read_call(std::fstream* fs, uintptr_t sig_offset)
{
	unsigned char bytes[12 + 1] = {};
	read_bytes(fs, (char*)bytes, sig_offset, 12);

	int rel_ptr = *(int*)(bytes + 8);

	uintptr_t ptr = rel_ptr + sig_offset + 12;

	return ptr;
}

size_t pattern_scan(std::fstream* fs, const char* sig, const char* mask, int length, int index, uint32_t file_size)
{
	char* buffer = new char[file_size];

	fs->seekg(0, std::ios::beg);
	fs->read(buffer, file_size);
		
	int currentindex = 1;
	uintptr_t offset = 0;
	for (int32_t i = 0; i < file_size - length; i++)
	{
		for (int32_t u = 0; u < length; u++)
		{
			if (sig[u] != buffer[u + i] && mask[u] == 'x')
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

				delete[] buffer;
				return i;
			}
		}
	}

	delete[] buffer;
	return 0;
}

void str_crypt(std::fstream* fs, const char* str, keystruct key, uint32_t file_size)
{
	uint32_t key_len = key.keysize;
	uint32_t p_len = strlen(str);
	char* mask = new char[p_len];
	for (size_t i = 0; i < p_len; i++)
	{
		mask[i] = 'x';
	}

	uint32_t offset = pattern_scan(fs, str, mask, p_len, 1, file_size);
	delete[] mask;

	if (offset == 0) { std::cerr << "Make sure you got your str_crypt strings right!\n"; }

	char* buffer = new char[0x1000 + 1];

	uint32_t str_len = 0;
	for (uint32_t u = 0;; u++)
	{
		read_bytes(fs, buffer, offset + (u * 0x1000), 0x1000);
		for (size_t i = 360; i < 0x1000; i++)
		{
			//std::cout << std::hex << (int)(unsigned char)buffer[i] << std::dec << '\n';
		}
		for (uint32_t i = 0; i < 0x1000; i++)
		{
			if (buffer[i] == '\x00') {
				str_len = i + (u * 0x1000);
				goto breaker;
			}
		}
	}
breaker:
	delete[] buffer;

	char* read_str = new char[str_len + 1];

	read_bytes(fs, read_str, offset, str_len);

	for (size_t i = 0; i < str_len; i++)
	{
		read_str[i] ^= key.key[i % key_len];
	}

	patch_bytes(fs, offset, read_str, str_len);
	delete[] read_str;
}

uintptr_t find_signature(std::fstream* fs, const char* sig, const char* mask, int length, int index, uint32_t file_size,
	uint32_t signature_id, uint32_t rdata_offset, uint32_t rdata_vaddr, uint32_t text_offset, uint32_t text_vaddr)
{
	char* id_c = i2c(signature_id);

	uintptr_t return_offset = 0;

	int current_index = 1;
	for (int i = 1; i < 100000; i++)
	{
		uintptr_t offset = pattern_scan(fs, sig, mask, length, i, file_size);
		if (offset == 0) { break; }

		uintptr_t id_ptr = read_ptr(fs, offset);
		uintptr_t func_ptr = read_call(fs, offset);

		const char func_pattern[] = "\x48\x89\x4C\x24\x08\x48\x83\xEC\x18\x48\xC7\x04\x24\x00\x00\x00\x00\xEB\x0B\x48\x8B\x04\x24";
		const char func_mask[] = "xxxxxxxx?xxx?xxxxxxxxxx";


		// mem offset of int
		id_ptr = (id_ptr - text_offset) + text_vaddr;

		// file offset
		id_ptr = (id_ptr + rdata_offset) - rdata_vaddr;


		if (id_ptr > file_size || func_ptr > file_size) { continue; }
		
		if (!match(fs, (const char*)id_c, "xxxx", 4, id_ptr)) { continue; }
		if (!match(fs, func_pattern, func_mask, 24 - 1, func_ptr)) { continue; }

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


