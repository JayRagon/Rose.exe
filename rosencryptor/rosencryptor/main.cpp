#include <iostream>
#include <windows.h>
#include <vector>
#include <chrono>

#include "file.h"
#include "structs.h"
#include "filemem.h"

#include "asm.h"


// the cleanest feature of rose yet
// it took me a while and a bit of confusion to find the std::ios::ate which was used to tell file size
// some idiot on stack overflow said it was a good idea
// i made a proper file size function now
// rosencryptor is one of my favourite parts of rose so far to make
// and it's SUPER easy to add encryption sections now
// just do k("key") in keystruct and everything works out for you
// absolutely amazing




/// [ i never ended up using zydis, just patching already placed NOPs from Rose.exe ]
/// [ this is because i just wanted a simple solution which didn't need to scale much ]
/// [ but this can definitely be improved ]
// alr
// to do list
// remove sigs from final build, patch find_signature calls with just the direct address
// e.g. find_signature(4) with mov ecx, addr of sig 4
// add ability to dynamically insert bytes into .text

// we just made file.bytes a vector, time to alter a relative instruction based on how many bytes we are inserting
// a mov [x], 0 instruction does not need *x to change, only x
// but a jmp [x] instruction needs *x to change by add [x], y and sub [x], y but also needs x to change



#define k(keystr) { keystr, sizeof keystr - 1 }
#define asm(bytes) { bytes, sizeof bytes - 1 }
#define sc(str, ki) { str, ki }

const char* c_str_path = "C:\\Users\\USER\\source\\repos\\Rose\\x64\\Release\\Rose.exe";
std::filesystem::path path = c_str_path;

int main()
{
	// offsets and vaddrs are used in dereferencing uintptr in .text
	PIMAGE_SECTION_HEADER data_header = find_section(c_str_path, ".data");
	PIMAGE_SECTION_HEADER rdata_header = find_section(c_str_path, ".rdata");
	PIMAGE_SECTION_HEADER text_header = find_section(c_str_path, ".text");
	PIMAGE_SECTION_HEADER pdata_header = find_section(c_str_path, ".pdata");
	

	const char pattern[] = "\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00";
	const char mask[] = "xxx????x????";

	std::fstream fs;

	fs.open(path, std::ios::in | std::ios::out | std::ios::binary);
	if (!fs.is_open()) {
		return 0xdead;
	}

	uint32_t file_size = get_file_size(&fs);
	std::cout << "STARTING FILE_SIZE: " << file_size << "\n\n";

	file_struct file_mem;
	//file_mem.bytes = new BYTE[file_size];
	//file_mem.size = file_size;
	file_mem.text = text_header;
	file_mem.data = data_header;
	file_mem.rdata = rdata_header;
	file_mem.pdata = pdata_header;
	file_mem.base_addr = get_base_addr(c_str_path);

	BYTE* read_buffer = new BYTE[file_size];
	read_bytes(&fs, (char*)read_buffer, 0, file_size);

	for (size_t i = 0; i < file_size; i++)
	{
		file_mem.bytes.push_back(read_buffer[i]);
	}

	delete[] read_buffer;

	/*
	assembly_init(&file_mem);
	__inject_asm(&file_mem, 0x487, "\x90", 1);


	// remove this if it's in another spot
	BYTE* patch_buffer = new BYTE[file_mem.bytes.size()];
	for (size_t i = 0; i < file_mem.bytes.size(); i++)
	{
		patch_buffer[i] = file_mem.bytes[i];
	}

	patch_bytes(&fs, 0, (const char*)patch_buffer, file_mem.bytes.size());

	Sleep(100000000);
	*/

	/*
	char* maskr_ = new char[file_size];
	for (size_t i = 0; i < file_size; i++)
	{
		maskr_[i] = 'x';
	}
	// basically checking if the file was read correctly
	// this is NEVER the case but it's for debugging
	if (!match(&fs, (const char*)file_mem.bytes, maskr_, file_size, 0)) {
		std::cout << "farts\n\n\nlikje bad fatrts smeelll\n";
	}
	delete[] maskr_;
	*/


	keystruct key[] =
	{
		k("\x75\x0D\xB9\x10\x00"),
		k("key"),
		k("\x1E\x73\x73\x03\xAD\xFF\x01\x62\x62\xE7"),
		k("bfjnrv"),
		k("fdfhjlnljhfdf"),
		k("nullhk0416"),
		k("\xE8\x94\x45\xBA\x05\x00\x00\x00"),
		k("D!jP?tP)~"),
		k("\x02\x02\x02\x00\x00\x00\x01\x03\x03\x03\x03\x03\x03\x01\x01\x01\x01\x01\x03\x03\x03\x01\x02\x02\x02\x02\x02\x00\x00\x03\x03\x01\x01\x01\x01\x01\x03\x03\x03\x03\x03\x03"),
		k("R0S3nQRxk"),
		k("~|vd.5bsidEso_RR_osEdisb5.dv|~~|vd.5bsidEso_RR_osEdisb5.dv|~"),
		k("Au_R3X0lRR>>40S3r[0]"),
		k("sub rsp, 48")
	};

	// xor '0'
	// 434552104243401c100408
	// xor 0x11
	// 525443015352510d011519

	strc strings[] =
	{
		sc("[The bouncer]\n\nWell done.|\nThe third task", 0),
		sc("[Adventures]\n\nWhat a playful bouncer.|\n", 1),
		sc("[The first encounter to a password]\n\nThen you save the traveller and", 2),
		sc("[The bouncer comes again]\n\nGood job.|\nNow it's starting", 3),
		sc("[Deception]\n\nYou can hack an obfuscated, simple game", 4),
		sc("Wrong.                        ", 4),
		sc("Correct.                        ", 4),
		sc("Password                        \n> ", 4),
		sc("[The chicken]\n\nImpressive. Very nice.\n", 5),
		sc("[A bouncer and his bytecode]\n\nWhat a dumb chicken.", 6),
		sc("[Great labyrinth]\n\nThe bouncer is keeping and eye on your improvement", 7),
		sc("You've been working in the dark this whole", 7),
		sc("            .\n        '    .  '\n", 7),
		sc("??|\nIt's really you?|\nI recognized you but", 7),
		sc("Anyways, I hope you like the candle", 7),
		sc("[Bouncer's arsenal]", 8),
		sc("[Virtual realm]", 9),
		sc("[Prison Break]", 10),
		sc("Bouncer: That... That's all i had left.", 10),
		sc("Bouncer: *sigh* Nope", 10),
		sc("You made it...|\nThrough", 11)
	};

	// string crypting
	uint32_t str_count = sizeof strings / sizeof strc;
	for (size_t i = 0; i < str_count; i++)
	{
		m_str_crypt(&file_mem, strings[i].str, key[strings[i].key_index]);
	}

	// inject inline assembly
	asmstruct assembly[] =
	{
		// [1] nopx3
		asm("\x90\x90\x90"),

		// [2] jmp -1, inc eax, sub rax 1, 9 byte nop
		asm("\xEB\xFF\xC0\x48\x83\xE8\x01\x66\x0F\x1F\x84\x00\x00\xF2\xF4\xF7"),

		// [3] jmp -1, inc eax, sub rax 1
		asm("\xEB\xFF\xC0\x48\x83\xE8\x01"),

		// [4] jmp -1, inc eax, sub rax 1
		asm("\xEB\xFF\xC0\x48\x83\xE8\x01"),

		// [5] jmp -1, inc eax, dec eax
		asm("\xEB\xFF\xC0\xFF\xC8"),

		// [6] jmp -1, inc eax, sub rax 1, 9 byte nop
		asm("\xEB\xFF\xC0\x48\x83\xE8\x01\x66\x0F\x1F\x84\x00\x00\xF2\xF4\xF7"),

		// [7] junk
		asm("\x48\x31\xD8\x48\x31\xC3\x48\x31\xD8\x50\x48\x83\xC4\x08\x48\x83\xF0\x01\x48\x21\xC0\x48\x83\xF0\x00\x48\x83\xF0\x01\x48\x31\xD8\x48\x31\xC3\x48\x31\xD8"),

		// [8] hidden call without broken assembly
		asm("\x53\x48\x83\xEC\x70\x48\xBB\x00\x00\x00\x00\x00\x00\x00\x00\xEB\x02\x48\xB8\xFF\xD3\x48\x83\xC4\x70\x5B\x90")
	};

	uint32_t asm_max_index = sizeof assembly / sizeof asmstruct;

	for (size_t i = 1; i <= asm_max_index; i++)
	{
		uint32_t offset = m_find_signature(&file_mem, pattern, mask, 12, 1, i + 0x1000);

		if (offset == 0) { std::cout << "bad\n"; }

		offset += 12;

		m_patch(&file_mem, assembly[i - 1].bytes, offset, assembly[i - 1].size);
	}



	// destroy sig 0xFFFF to indicate that the file has been encrypted
	uintptr_t destroy_offset = m_find_signature(&file_mem, pattern, mask, 12, 1, 0xFFFF);
	m_patch(&file_mem, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", destroy_offset, 12);


	uint32_t index_max = (sizeof key / sizeof keystruct);

	for (uint32_t i = 1; i <= index_max; i++)
	{
		uint32_t offset = m_find_signature(&file_mem, pattern, mask, 12, 1, i) + 12;
		uint32_t end_offset = m_find_signature(&file_mem, pattern, mask, 12, 2, i);

		uint32_t section_size = end_offset - offset;

		//std::cout << "size: " << section_size << '\n';

		/*
		std::cout << "start: " << offset << '\n';
		std::cout << "end: " << end_offset << '\n';
		*/

		if (section_size > 0x100000) {
			std::cout << "ERR\n";
			section_size = 0;
		}

		for (size_t u = 0; u < section_size; u++)
		{
			file_mem.bytes[u + offset] ^= key[i - 1].key[u % (key[i - 1].keysize)];
		}
	}

	BYTE* patch_buffer = new BYTE[file_mem.bytes.size()];
	for (size_t i = 0; i < file_mem.bytes.size(); i++)
	{
		patch_buffer[i] = file_mem.bytes[i];
	}

	patch_bytes(&fs, 0, (const char*)patch_buffer, file_mem.bytes.size());
	fs.close();

	std::cout << "\nENDING FILE_SIZE: " << file_mem.bytes.size() << "\n\n";
	std::cout << "BYTE DELTA: " << (int32_t)(file_mem.bytes.size() - file_size) << "\n\n";

	delete[] patch_buffer;
}



