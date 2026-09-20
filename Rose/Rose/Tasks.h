#pragma once

#include "mem.h"
#include "print.h"
#include "cout_color.h"

#include "hashing.hpp"
#include "strcryptx.hpp"
#include "intcrypt.h"

#include "inline_asm.h"
#include "obfuscation.h"

#include <Windows.h>
#include <chrono>
#include <time.h>

// this is for the return address spoofing (if i recall correctly...)
#pragma section(".text")
__declspec(allocate(".text")) const BYTE jmp_rbx[] = { 0xFF, 0x23 };

// 1 (easy-)
__forceinline void first_stage()
{
	for (;;)
	{
		if (*(uintptr_t*)&print == 5) {
			break;
		}
		Sleep(100);
	}
}

// this task was actually to force the user to know how to change the print speed of the game
// so that if they don't like it, or it's too slow, they can literally change it themselves!
// this was to kinda open up the player to enjoy reverse engineering
// 2 (easy)
bool patch_print_check(unsigned char* bytes)
{
	auto hash = ch_a("\x89\x54\x24\x10\x48\x89\x4c\x24\x8\x48\x83\xec\x48\xc7\x44\x24\x20\x40\x30\x0\x0\x48\xc7\x44\x24\x28\x0\x0\x0\x0\xeb\xd\x48\x8b\x44\x24\x28\x48\xff\xc0\x48\x89\x44\x24\x28\x48\x8b\x44\x24\x28\x48\x8b\x4c\x24\x50\x48\x3\xc8\x48\x8b\xc1\xf\xbe\x0\x85\xc0\x74\x50\x48\x8b\x44\x24\x28\x48\x8b\x4c\x24\x50\x48\x3\xc8\x48\x8b\xc1\xf\xbe\x0\x83\xf8\x7c\x75\xd\xb9\x10\x00");

	unsigned char mem[95];

	for (size_t i = 0; i < 95; i++)
	{
		mem[i] = deref8((uintptr_t)&print + i);
	}

	for (size_t i = 0; i < 95; i++)
	{
		bytes[i] = mem[i];
	}

	ch_r(mem, sizeof mem);

	bool match = true;
	for (size_t i = 0; i < 96 - 1; i++)
	{
		if (mem[i] != hash.data[i]) {
			match = false;
			break;
		}
	}

	// mov:
	// 5C 5D 5E 5F 60
	// B9 ?? ?? ?? ??
	constexpr uintptr_t offset = 0x5c + 1;

	if (match)
	{
		DWORD dummy0;
		VirtualProtectEx(phandle, (LPVOID)((uintptr_t)&print + offset), 2, PAGE_EXECUTE_READWRITE, &dummy0);
		deref16((uintptr_t)&print + offset) = 0x012c;
		VirtualProtectEx(phandle, (LPVOID)((uintptr_t)&print + offset), 2, PAGE_EXECUTE_READ, &dummy0);
		return true;
	}
	return false;
}

// 3 (easy+)
__forceinline void first_run_check(void* main_func, char* run_check)
{
	bool jmps[4] = {};

	set_cout_color(COUT_RED);
	std::cout << "\nBouncer: You ain't invited.\n";
LOOP_FUNC:

	for (;;)
	{
		if (*(uintptr_t*)&print <= 0xdeafbeef) {
			break;
		}
		Sleep(84);
	}

	if (*(uintptr_t*)&print > 0xdeadbeef) {
		if (!jmps[0]) {
			std::cout << "Bouncer: Good try, but I saw that.\n";
			jmps[0] = true;
		}
		goto LOOP_FUNC;
	}

	if ((uintptr_t)&print > 0xdeffbeef)
	{
		run_check[0] = 'k';
		run_check[1] = 'e';
	}


	if (*(uintptr_t*)&print >= 0xdeffaaaa) {
		if (!jmps[1]) {
			std::cout << "Bouncer: Do yourself a favor and quit tryin'.\n";
			jmps[1] = true;
		}
		goto LOOP_FUNC;
	}

	if (*(uintptr_t*)&print > 0xdeadbeef) {
		if (!jmps[2]) {
			std::cout << "Bouncer: Heh, almost got me. But not quite. Go find yo parents or somethin'\n";
			jmps[2] = true;
		}
		goto LOOP_FUNC;
	}

	if ((uintptr_t)&print != 0xdeadbeef) {
		std::cout << "Bouncer: Whoa, you really jus' done that? Gotta give respect where iss due.\n";
		run_check[2] = 'y';
	}
	else {
		if (!jmps[3]) {
			std::cout << "Bouncer: Nah, not it. I wanna see some real effort before I let you in.\n";
		}
		jmps[3] = true;
		goto LOOP_FUNC;
	}
	
	auto hash = ch_a("key");

	unsigned char run_check_cpy[3] = { run_check[0], run_check[1], run_check[2] };
	ch_r((unsigned char*)run_check_cpy, 3);

	if (ch_cmp(run_check_cpy, hash.data, 3)) {
		std::cout << "Bouncer: Alright, alright, I'll give you that one for the effort.\n\n";
		set_cout_color(COUT_GREEN);
	}
	else {
		std::cout << "Bouncer: Whoa, you skipped a step! The club gonna crash if you do that!\n";
		std::cout << "Sketchy guy smoking: Ey, you, yeah you, ya probably jumped over too much and missed somethin' critical.\n"
			"Don't skip things unessecarily 'round here.\nJus' restart n' everythin' should be all good again.\n";
		Sleep(100000000);
	}
}

// 4 (easy++)
__forceinline void first_game(BYTE run_check[10])
{
	uint32_t gold = iobf_r(dcri(iobf_c(3) ^ 0xC1A55));
	uint32_t damage = iobf_r(dcri(iobf_c(0) ^ 0x1F1ED));
	int32_t health = iobf_r(dcri(iobf_c(100) ^ 0x0EADBEEF));

	uint32_t kills = iobf_r(dcri(iobf_c(0) ^ 0xC1A55));

	uint32_t gold_backup = gold;
	uint32_t damage_backup = damage;
	uint32_t health_backup = health;

	bool broken_sword_bought = false;
	bool copper_sword_bought = false;
	bool gold_sword_bought = false;

	set_cout_color(COUT_RED);
	
	std::cout << "Hey traveller! I've fallen down into this cave and it looks like you have too.\n"
		"I'm too weak to fight those big scary monsters to get home. Please kill them and get me out!\n\n";
	for (;;)
	{
		std::cout << "Options:\nboss\nfight\nshop\nheal\nstats\n> ";

		std::string option;
		std::cin >> option;
		std::cout << '\n';
		if (option == "fight")
		{
			int32_t enemy_hp = iobf_r(rand() % dcri(iobf_c(100)));
			uint32_t enemy_dmg = 20;

			set_cout_color(COUT_RED);
			
			bool died;
			for (;;)
			{
				if (dcri(iobf_r(dcri(health ^ 0x0EADBEEF))) <= 0) {
					died = true;
					break;
				}
				if (dcri(iobf_r(dcri(enemy_hp))) <= 0) {
					died = false;
					break;
				}

				std::cout << "Enemy HP:  " << dcri(iobf_r(dcri(enemy_hp))) << '\n';
				std::cout << "Enemy DMG: " << enemy_dmg << "\n\n";

				std::cout << "Your HP:   " << dcri(iobf_r(dcri(health ^ 0x0EADBEEF))) << '\n';
				std::cout << "Your DMG:  " << dcri(iobf_r(dcri(damage ^ 0x1F1ED))) << "\n\n";

				health   = iobf_r(dcri(health ^ 0x0EADBEEF) - enemy_dmg) ^ 0x0EADBEEF;
				enemy_hp = iobf_r(dcri(enemy_hp) - dcri(damage ^ 0x1F1ED));

				std::cout << cryptz("You both attack!\n\n", "r?").dec("r?");

				Sleep(200);
			}

			set_cout_color(COUT_RED);

			if (died) {
				health = iobf_r(dcri(iobf_c(0) ^ 0x0EADBEEF));
				std::cout << "You died!\n\n";
			}
			else {
				gold = iobf_r(dcri(gold ^ 0xC1A55) + dcri(iobf_c(10))) ^ 0xC1A55;
				kills = iobf_r(dcri(iobf_r(dcri(kills ^ 0xC1A55) + 1))) ^ 0xC1A55;
				std::cout << "You got 10 gold and 1 more kill!\n\n";
			}
		}
		else if (option == "boss")
		{
			run_check[7] = option[0];
			if (dcri(iobf_r(dcri(kills ^ 0xC1A55))) >= dcri(iobf_c(10000)))
			{
				set_cout_color(COUT_RED);
				if ((uintptr_t)&first_game <= dcri(kills ^ 0xC1A55)) {
					std::cout << "Boss HP:  ";
					std::cout << "Boss DMG: ";

					std::cout << "Your HP:  ";
					std::cout << "Your DMG: ";
					std::cout << cryptz("\nRoses are red\nViolets are blue\n"
						"And I, the boss\nDid not invite you.\n\n", "R3PMUJ").dec("R3PMUJ");
				}
				else if (dcri(iobf_r(dcri(kills ^ 0xC1A55)) + 10) >= dcri(iobf_c(10010)))
				{
					run_check[1] = option[3];
					run_check[2] = option[2];
					run_check[8] = option[0];

					bool died;

					int32_t enemy_hp = iobf_c(0xBADBEEF) ^ 0xB1A2E;
					uint32_t enemy_dmg = iobf_c(0xBADBEEF) ^ 0xF14E;

					run_check[6] = 0x01;

					for (;;) 
					{
						if (dcri(health ^ 0x0EADBEEF) <= dcri(iobf_c(0)))
						{
							died = true;
							break;
						}
						if (dcri(enemy_hp ^ 0xB1A2E) <= dcri(iobf_c(0)))
						{
							died = false;
							break;
						}

						std::cout << "Boss HP:  " << dcri(enemy_hp ^ 0xB1A2E) << '\n';
						std::cout << "Boss DMG: " << dcri(enemy_dmg ^ 0xF14E) << "\n\n";

						std::cout << "Your HP:  " << dcri(health ^ 0x0EADBEEF) << '\n';
						std::cout << "Your DMG: " << dcri(damage ^ 0x1F1ED) << "\n\n";

						health = iobf_r(dcri(health ^ 0x0EADBEEF) - dcri(enemy_dmg ^ 0xF14E)) ^ 0x0EADBEEF;
						enemy_hp = iobf_r(dcri(enemy_hp ^ 0xB1A2E) - dcri(damage ^ 0x1F1ED)) ^ 0xB1A2E;

						std::cout << cryptz("You both attack!\n\n", "o?").dec("o?");
					}

					run_check[0] = 0x1E;
					run_check[3] = 0x03;
					run_check[9] = 0xE7;
					
					if (died) {
						std::cout << "\nFool. I am the boss. I am the king. You may never leave the cave\n\n";
					}
					else if (enemy_hp <= dcri(iobf_c(0))) 
					{
						uintptr_t addr_obf = iobf_r(damage) ^ iobf_r(damage);
						addr_obf += (uintptr_t)run_check + 4;
						*(BYTE*)addr_obf = 0xAD;
						addr_obf++;
						*(BYTE*)addr_obf = 0xFF;

						std::cout << cryptz("\nAAAHHHH\nWHAT WAS THAT!!!! HOW HAS THIS HAPPENED!!! AAAHHH\n\n", "King's defeat").dec("King's defeat");
						set_cout_color(COUT_GREEN);
						return;
					}
					else {
						std::cout << "Hang on, I didn't die.\nDid you seriously just try to convince me that I died so that you could get past?\n\n";
					}
				}
				else
				{
					std::cout << "Nice try. Ten thousand kills required.\n\n";
				}
				set_cout_color(COUT_RED);
			}
			else
			{
				std::cout << "Ten thousand kills required. I won't bother fighting you until then.\n\n";
			}
		}
		else if (option == "stats")
		{
			std::cout << "Damage: " << dcri(iobf_r(dcri(damage ^ 0x1F1ED))) << '\n';
			std::cout << "Health: " << dcri(iobf_r(dcri(health ^ 0x0EADBEEF))) << '\n';
			std::cout << "Gold:   " << dcri(iobf_r(dcri(gold ^ 0xC1A55))) << '\n';
			std::cout << '\n';
		}
		else if (option == "shop")
		{
			std::cout << "You have " << dcri(gold ^ 0xC1A55) << " gold.\n";
			std::cout << "Purchase options (one time only):\n";
			if (!broken_sword_bought) { std::cout << "broken_sword, 3 gold\n"; }
			if (!copper_sword_bought) { std::cout << "copper_sword, 50 gold\n"; }
			if (!gold_sword_bought)   { std::cout << "gold_sword, 100000 gold\n> "; }

			std::string sword_choice;
			std::cin >> sword_choice;
			std::cout << '\n';

			if (sword_choice == "broken_sword")
			{
				if (dcri(gold ^ 0xC1A55) >= 3 && dcri(gold ^ 0xC1A55) > 2
					&& (dcri(gold ^ 0xC1A55) & 0) == 0 && dcri(gold ^ 0xC1A55) + 3 > 2
					&& !broken_sword_bought)
				{
					gold = iobf_r(dcri(gold ^ 0xC1A55) - 3) ^ 0xC1A55;
					damage = iobf_r(dcri(damage ^ 0x1F1ED) + 10) ^ 0x1F1ED;
					broken_sword_bought = true;
				}
				else {
					std::cout << "You are poor. Go fight some more monsters.\n\n";
				}
			}
			else if (sword_choice == "copper_sword")
			{
				if (dcri(gold ^ 0xC1A55) >= 50 && dcri(gold ^ 0xC1A55) > 49
					&& (dcri(gold ^ 0xC1A55) & 0) == 0 && dcri(gold ^ 0xC1A55) + 3 > 2
					&& !copper_sword_bought)
				{
					gold = iobf_r(dcri(gold ^ 0xC1A55) - 50) ^ 0xC1A55;
					damage = iobf_r(dcri(damage ^ 0x1F1ED) + 100) ^ 0x1F1ED;
					copper_sword_bought = true;
				}
				else {
					std::cout << "You are poor. Go fight some more monsters.\n\n";
				}
			}
			else if (sword_choice == "gold_sword")
			{
				if (dcri(gold ^ 0xC1A55) >= 100000 && dcri(gold ^ 0xC1A55) > 100000 - 1
					&& (dcri(gold ^ 0xC1A55) & 0) != 1 && dcri(gold ^ 0xC1A55) + 3 > 5
					&& !gold_sword_bought)
				{
					gold = iobf_r(dcri(gold ^ 0xC1A55) - 100000) ^ 0xC1A55;
					damage = iobf_r(dcri(damage ^ 0x1F1ED) + 1000) ^ 0x1F1ED;
					gold_sword_bought = true;
				}
				else {
					std::cout << "You are poor. Go fight some more monsters.\n\n";
				}
			}
			std::cout << "Damage now: " << dcri(iobf_r(dcri(damage ^ 0x1F1ED))) << "\n\n";
		}
		else if (option == "heal")
		{
			health = dcri(health ^ 0x0EADBEEF) + 10;
			if (health > 200) { health = 200; }
			health = iobf_r(health) ^ 0x0EADBEEF;

			std::cout << "Health: " << dcri(health ^ 0x0EADBEEF) << "\n\n";
		}
		else if (option == cryptz("JUMP3R", "Bladeburners 2079").dec("Bladeburners 2079"))
		{
			// SECRET_DIALOGUE
			set_cout_color(COUT_GREEN);
			print(cryptz("Found a secret have you?|\nInteresting.|.|.|\n"
				"Many would have done the task and not explore.|\n"
				"But you decided to have a little bit of fun didn't you?|\n"
				"Have some fun, take your time.|\n"
				"You are doing a great job.|\n\n",
				"JUMP3R").dec(option.c_str()));
			set_cout_color(COUT_RED);
		}
	}
}

// 5 (medium)
__forceinline bool first_keygen(char key[30], int kl)
{
	// kl check
	if (kl < 6 || kl > 12) { return false; }

	// check all lowercase
	for (size_t i = 0; i < kl; i++)
	{
		if (key[i] < 'a' || key[i] > 'z')
		{
			return false;
		}
		key[i] -= 'a';
		if (key[i] % 2 == 0) {
			return false;
		}
		if (i > 0) {
			if (key[i - 1] + 4 != key[i]) {
				return false;
			}
		}
	}

	return true;
}

// 6 (medium+)
__forceinline bool second_keygen(char key[30], int kl)
{
	if (kl != 13) { return false; }

	if (key[(kl - 1) / 2] != 'n') { return false; }
	if (key[1] != 'd') { return false; }

	for (size_t i = 0; i < kl; i++)
	{
		key[i] -= 'a';
	}

	for (size_t i = 0; i < kl; i++)
	{
		if (key[i] != key[kl - i - 1]) { return false; }

		if (i < kl - 1)
		{
			if (key[i] + 2 != key[i + 1] && key[i] - 2 != key[i + 1]) { return false; }
		}

		if (key[i] < 3) { return false; }
	}

	return true;
}

//                0123456789
// 7 (medium++)   nullhk0416
__declspec(noinline) BOOL hidden_check_imp_disassembly(HANDLE z1, LARGE_INTEGER ptr, PLARGE_INTEGER z2, UINT z3)
{
	if (ptr.QuadPart == 0) { return false; }
	volatile bool x = true;
	char* key = (char*)ptr.QuadPart;

	if (key[7] != '4') { x = false; }
	if (key[5] != 'k') { x = false; }
	volatile bool obf = key[7] != '3';
	if ((key[6] ^ key[9]) != 6) { x = false; } // key[6] must be '0'
	if (key[8] != '1') { x = false; }
	obf = key[9] != '3';
	volatile bool obf2 = key[4] != 'k';
	obf2 = key[4] != 'j';
	if (key[4] != 'h') { x = false; }
	inline_asm(7, 38); // junk code
	if (key[9] != key[8] + 5) { x = false; } // key[9] must be '6'
	if (strlen(key) != 10) { x = false; }

	return x;
}
__forceinline bool imp_disassembly(char key[30])
{
	volatile bool result = true;

	inline_asm(2, 16);
	if (key[0] != 'n') { result = false; }

	volatile bool obf = false;
	obf = key[1] == 'k';
	obf = key[1] != 'n';

	volatile uint32_t obf2 = key[4] + key[5];
	obf2 ^= 0xf5474;
	obf2 += key[0];

	inline_asm(3, 7);
	volatile bool check = (key[0] ^ key[1]) == 0x1b; // if key[1] == 'u'
	if (!check) { result = false; }
	
	inline_asm(4, 7);
	check = key[2] == key[3];
	if (!check) { result = false; }
	check = key[2] == 'l';
	if (!check) { result = false; }

	// checked for null

	inline_asm(5, 5);
	obf2 = key[4] ^ 'k';
	obf = obf2 == 0xd;


	uintptr_t og_func_addr;
	IAT::hook("SetFilePointerEx", (uintptr_t)hidden_check_imp_disassembly, &og_func_addr);
	LARGE_INTEGER ptr;
	ptr.QuadPart = (uint64_t)key;

	inline_asm(6, 16);
	bool buffer = SetFilePointerEx(nullptr, ptr, nullptr, NULL);
	IAT::unhook("SetFilePointerEx", og_func_addr);

	if (!buffer) {
		result = false;
	}

	return result;
}

// 8 (hard--)
__declspec(noinline) BOOL hidden_perfcounter(HANDLE z1, LARGE_INTEGER z2, PLARGE_INTEGER ptr, DWORD array_offset) 
{
	QueryPerformanceCounter(&ptr[array_offset]);
	Sleep(1);
	return FALSE;
}
__declspec(noinline) void seh_loop_chicken(LARGE_INTEGER* check, LARGE_INTEGER* times)
{
	uintptr_t fileptr_addr = 0;
	IAT::hook("SetFilePointerEx", (uintptr_t)hidden_perfcounter, &fileptr_addr);

	volatile uint32_t inc = 0;

// to fix the (inc != 1) getting optimized out
ANTI_OPTI_LABEL:
	__sig(0x10000);
	LARGE_INTEGER large_null = {};
	
	if (inc != 1) {
		__try {
			SEH::direct::trigger_exception(0x10001, (uintptr_t)seh_loop_chicken);
		}
		__except(1) {
			// this should also never happen, because it's hooked to skip doing PerfQuery on check
			std::cout << "Chicken Farts!\n";
		}
	}
	SetFilePointerEx(nullptr, large_null, check, 0);

	__sig(0x10001);
	SetFilePointerEx(nullptr, large_null, times, inc);

	inc++;
	if (inc < 6) {
		__try {
			SEH::direct::trigger_exception(0x10000, (uintptr_t)seh_loop_chicken);
		}
		__except(1) {
			// this should never happen, since it's been hooked to loop
			std::cout << "Chicken lives!\n";

			// to make sure that the if statement always gets checked, however this will never happen due to SEH hook
			goto ANTI_OPTI_LABEL;
		}
	}

	SEH::direct::unhook();
	IAT::unhook("SetFilePointerEx", fileptr_addr);
}
__declspec(noinline) WORD hidden_integrity_check(DWORD z1, DWORD z2, PVOID* ptr, PDWORD z3)
{
	if (((LARGE_INTEGER*)ptr)->QuadPart != (LONGLONG)(18444444044487591615 ^ 0x2079)) {
		exit(1);
		return 0;
	}
	return 1;
}
__declspec(noinline) uint64_t hide_checks_chicken(LARGE_INTEGER check, LARGE_INTEGER* times)
{
	volatile uint64_t required_tick = 18444444044487591615;

	SEH::direct::hook(0x10004, (uintptr_t)hide_checks_chicken, SEH::direct::get_rsp());

	__try {
		uintptr_t nuke_addr = find_signature((uintptr_t)hide_checks_chicken, 0x10002, 1);
		patchmem(nuke_addr, random4096, 0x1000);
		__sig_safe(0x10002);
		__inline_asm(0x1000);
	}
	__except (1) {
		std::cout << "pain exponentiel avec du poisson\n";
	}

	__sig(0x10003);
	LARGE_INTEGER check2;
	QueryPerformanceCounter(&check2);
	if (check2.QuadPart - times[0].QuadPart > 10000000)
	{
		printf(cryptx_obf("chicken refuses\n\n", "Ref-us/al."));
		return 0;
	}

	if (check.QuadPart == required_tick && times[0].QuadPart < times[1].QuadPart &&
		times[4].QuadPart > times[3].QuadPart && times[2].QuadPart < times[5].QuadPart &&
		check.QuadPart > times[0].QuadPart && check.QuadPart < times[4].QuadPart)
	{
		RtlCaptureStackBackTrace(0, 0, (PVOID*)&check, nullptr);

		std::cout << cryptx_obf("        |    |    |     O-\n"
			"        |         |   _/| \n"
			"        |    |    |   [ [ \n\n", "XORRR");

		std::cout << cryptx_obf("You did it!\n\n", "\x33\x50\xC7\x00\x03\xB0");
		__try {
			SEH::direct::trigger_exception(0x10006, (uintptr_t)hide_checks_chicken);
		}
		__except (1) {
			std::cout << "\n";
		}
		__sig(0x10007);
		return (check.LowPart + check.HighPart * (check.QuadPart >> 52)) << (check.QuadPart & 3);
	}
	else {
		std::cout << cryptx_obf("        | [  | O  |\n"
			"        |  / \\  - |\n"
			"        |    |  ] |\n\n", "xorrrrrr");

		std::cout << cryptx_obf("The chicken would've gotten brutally run over by an oncoming car.\n"
			"Try again.\n\n", "\x93\x10\xA7\x7c\x03\xEB");
	}
	__try {
		SEH::direct::trigger_exception(0x10005, (uintptr_t)hide_checks_chicken);
	}
	__except (1) {
		std::cout << "R0S3\n";
	}

	__sig(0x10006);
	check.QuadPart ^= 0x2079;
	__try {
		SEH::direct::trigger_exception(0x10007, (uintptr_t)hide_checks_chicken);
	}
	__except (1) {
		std::cout << "R0S3\n";
	}

	__sig(0x10004);
	required_tick ^= 0x2079;
	check.QuadPart ^= 0x2079;
	__try {
		SEH::direct::trigger_exception(0x10003, (uintptr_t)hide_checks_chicken);
	}
	__except (1) {
		std::cout << "r\n";
	}

	__sig(0x10005);
	SEH::direct::unhook();
	return 0;
}
__declspec(noinline) uint64_t chicken2(LARGE_INTEGER check, LARGE_INTEGER* times)
{
	uint64_t result = 0;

	// impossible disassembly is in play here, just from the main chicken() function
	result = hide_checks_chicken(check, times);

	volatile int x = 0;
	return result;
}
__forceinline uint64_t chicken()
{
	set_cout_color(COUT_RED);

	patchkit imp((uintptr_t)chicken2 - 2, 2);
	imp.patch("\x48\xA3"); // creates a super long instruction just by adding this

	std::cout <<
		"   O-   |    |    |\n"
		" _/|    |         |\n"
		" [ [    |    |    |\n\n";

	uintptr_t rtlcapture_addr;
	IAT::hook(cryptx_obf("RtlCaptureStackBackTrace", "BLANKFIELD"), (uintptr_t)hidden_integrity_check, &rtlcapture_addr);

	for (;;) 
	{
		std::cout << "tell the chicken to go\n> ";
		std::string input;
		std::cin >> input;

		if (input == "go")
		{
			LARGE_INTEGER times[6];
			LARGE_INTEGER check;

			seh_loop_chicken(&check, times);

			uint64_t result = chicken2(check, times);
			if (result == 0) {
				continue;
			}
			IAT::unhook(cryptx_obf("RtlCaptureStackBackTrace", "rrrose"), rtlcapture_addr);
			return result;
		}
		else if (input == cryptx_obf("Daedalus", "moonlight"))
		{
			std::cout << cryptx_obf("*chicken says something about the time*\n\n", "xorrr");
		}
		else if (input == cryptx_obf("run4theh111z", "Foreground Eclipse"))
		{
			std::cout << cryptx_obf("\"Quack!\"\n\n", "xor");
		}
		else if (input == "hint") {
			std::cout << "*chicken tells you about the exceptions causing a jmp to an unknown location*\n*he also tells you to step into, instead of stepping over, whatever that means*\n\n";
		}
	}


	set_cout_color(COUT_GREEN);
	return 0;
}

// 9 (hard-)
// in virtualization::t9

// 10 (hard)
namespace second_game
{
	__declspec(noinline) uintptr_t ras_ex()
	{
		void* ret = _ReturnAddress();
		std::cout << "return address spoofing\nret: 0x" << ret << '\n';
		return (uintptr_t)ret;
	}

	void call_voidptr(void* func_ptr)
	{
		void(*func)() = 0;
		func = (decltype(func))func_ptr;
		func();
	}

	enum {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	std::vector<byte> directions;

	HHOOK hook_handle = NULL;
	volatile bool finish_game = false;

	LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			KBDLLHOOKSTRUCT* pkb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
			HWND foreground_window = GetForegroundWindow();
			DWORD vk = pkb->vkCode;

			if (wParam == WM_KEYDOWN)
			{
				switch (vk)
				{
				case VK_UP:
					directions.push_back(UP);
					break;
				case VK_DOWN:
					directions.push_back(DOWN);
					break;
				case VK_LEFT:
					directions.push_back(LEFT);
					break;
				case VK_RIGHT:
					directions.push_back(RIGHT);
					break;
				}
			}
		}

		// call the next keyboard hook so that other things can function
		return CallNextHookEx(hook_handle, nCode, wParam, lParam);
	}

	// setup and maintain the keyboard hook
	void setup_kbhook()
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);
		hook_handle = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, hInstance, 0);

		if (!hook_handle) {
			std::cerr << "Failed to install hook!" << std::endl;
			exit(0xdead);
		}

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0) > 0 && !finish_game) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UnhookWindowsHookEx(hook_handle);
	}


	int wallsx[] = { 1, -1,  0, -2, 2, 0, 2,  2,  1,  -1, -4, -4, -4, -4, -3, -2, -1, 0, 2, 2, 5, 5, 5 };
	int wallsy[] = { 1,  1, -1,  1, 1, 1, 0, -1, -1,  -1,  1,  2,  0, -1,  3,  3,  3, 3, 2, 3, 4, 5, 6 };
	constexpr int walls = sizeof wallsx / 4;

	int endx = 9;
	int endy = 7;

	class wall_dtc_dec {
	public:
		virtual bool w1(int x, int y, int i, void* classptr) = 0;
		virtual bool w2(int x, int y, int i, void* classptr) = 0;
		virtual bool w3(int x, int y, int i, void* classptr) = 0;
		virtual bool w4(int x, int y, int i, void* classptr) = 0;
	};

	class wall_dtc : public wall_dtc_dec {
	public:
		virtual bool w1(int x, int y, int i, void* classptr) override 
		{
			if (*(uint64_t*)classptr == 0x94839043982) {
				__sig_safe(0x10008);
				SEH::direct::unhook();
				return true;
			}
			uintptr_t seh_hook_addr = vtable::get_vtable(classptr)[0];

			for (size_t w = 0; w < walls; w++)
			{
				if (x == wallsx[w] && y == wallsy[w]) 
				{
					int ogsize = directions.size();
					for (size_t k = 0; k < ogsize - i; k++)
					{
						directions.pop_back();
					}
					__try {
						SEH::direct::trigger_exception(0x10008, seh_hook_addr);
					}
					__except (1) {
						Sleep(1);
					}
					return false;
				}
			}
			return false;
		}

		virtual bool w2(int x, int y, int i, void* classptr) override {
			volatile int ogsize = directions.size();
			volatile void* z = classptr;
			for (volatile size_t w = 0; w < walls * 2; w += 2)
			{
				if (x == wallsx[w >> 1] && y == wallsy[w >> 1]) {
					for (size_t k = 0; k < ogsize - i; k++)
					{
						volatile int D = 0xdead;
						volatile int A = 0xdead;
						A = 0x7780;
						A = 0x7781;
						A = 0x7782;
						A = 0x7783;
						A = 0x7784;
						directions.pop_back();
						volatile int E = 0xdead;
						volatile int G = 0xdead;
						volatile int C = 0xdead;
						volatile int B = 0xdead;
					}
					return true;
				}
			}
			return false;
		}

		virtual bool w3(int x, int y, int i, void* classptr) override {
			bool boolx = false;

			volatile int ogsize = directions.size();
			volatile void* z = classptr;
			for (volatile size_t w = 0; w < walls * 4; w += 4)
			{
				if (x == wallsx[w >> 2] && y == wallsy[w >> 2]) {
					for (size_t k = 0; k < ogsize - i; k++)
					{
						directions.pop_back();
					}
					boolx = true;
					break;
				}
			}

			volatile bool result = boolx;
			return result;
		}

		virtual bool w4(int x, int y, int i, void* classptr) override {

			bool boolx = false;

			volatile int ogsize = directions.size();
			volatile void* z = classptr;
			for (volatile size_t w = 0; w < walls * 2; w += 2)
			{
				if (x == wallsx[w >> 1] && y == wallsy[w >> 1]) {
					for (size_t k = 0; k < ogsize - i; k++)
					{
						volatile int D = 0xdead;
						volatile int A = 0xdead;
						A = 0x7780;
						A = 0x7781;
						A = 0x7782;
						A = 0x7783;
						A = 0x7784;
						directions.pop_back();
						volatile int E = 0xdead;
						volatile int G = 0xdead;
						volatile int C = 0xdead;
						volatile int B = 0xdead;
					}
					boolx = true;
					break;
				}
			}

			volatile bool result = boolx;

			volatile bool result2 = false;
			if (*(uint64_t*)classptr == 0x94839043982) {
				__sig_safe(0x10008);
				SEH::direct::unhook();
				result2 = true;
				return result | result2;
			}
			uintptr_t seh_hook_addr = vtable::get_vtable(classptr)[0];

			for (size_t w = 0; w < walls; w++)
			{
				if (x == wallsx[w] && y == wallsy[w])
				{
					int ogsize = directions.size();
					for (size_t k = 0; k < ogsize - i; k++)
					{
						directions.pop_back();
					}
					__try {
						SEH::direct::trigger_exception(0x10008, seh_hook_addr);
					}
					__except (1) {
						Sleep(1);
					}
					result2 = false;
				}
			}

			return result | result2;
		}
	};

	__forceinline bool hitting_wall(int x, int y, int i, volatile bool* check, uintptr_t main_addr)
	{
		int ogsize = directions.size();
		volatile bool result = false;
		wall_dtc* dtc = new wall_dtc;

		*check = true;

		uintptr_t og_addr = 0;
		uintptr_t* dtc_vtable = vtable::get_vtable(dtc);
		uintptr_t hook_addr = dtc_vtable[timing::qperfraw() % 4];

		vtable::hook(dtc, 0, (void*)hook_addr, &og_addr);
		if (dtc->w1(x, y, i, dtc)) {
			result = true;
		}
		vtable::unhook(dtc, 0, og_addr);
		delete dtc;
		return result;
	}

	void evil(uintptr_t* vtable)
	{
		patchkit imp_spam[] = {
			patchkit(0x1000 + 0, 7, vtable[0]),
			patchkit(0x1000 + 1, 7, vtable[1]),
			patchkit(0x1000 + 2, 7, vtable[2]),
			patchkit(0x1000 + 3, 7, vtable[3])
		};

		uint64_t current;
		uint64_t last = 0;
		while (!finish_game)
		{
			for (size_t i = 0; i < 4; i++)
			{
				current = timing::qperfc();
				if ((current - last) / 1000 > 20 && last) {
					ExitProcess(0xB1ADE5);
				}

				switch (timing::qperfraw() % 7)
				{
				case 0:
					imp_spam[i].patch("\x90\x90\x90\x90\xEB\x01\xA1");
					break;
				case 1:
					imp_spam[i].patch("\x90\x90\x90\xEB\x02\xC7\x80");
					break;
				case 2:
					imp_spam[i].patch("\x90\x90\xEB\x03\x48\xC7\x80");
					break;
				case 3:
					imp_spam[i].patch("\x90\xEB\x04\x48\xC7\x84\xC8");
					break;
				case 4:
					imp_spam[i].patch("\x90\x90\x90\xEB\x02\x0F\x85");
					break;
				case 5:
					imp_spam[i].patch("\x90\x90\x90\x90\xEB\x01\xB8");
					break;
				case 6:
					imp_spam[i].patch("\x90\x90\x90\x90\xEB\x01\x0F");
					break;
				}

				last = timing::qperfc();
				Sleep(1);
			}
		}
	}
	__declspec(noinline) void setup_evil(uintptr_t* vtable)
	{
		if (vtable) {
			std::thread evil_thread([vtable]() {
				evil(vtable);
				});
			evil_thread.detach();
		}

		patchkit imp_spam[] = {
			patchkit(0x1000 + 0, 7, vtable[0]),
			patchkit(0x1000 + 1, 7, vtable[1]),
			patchkit(0x1000 + 2, 7, vtable[2]),
			patchkit(0x1000 + 3, 7, vtable[3])
		};

		uint64_t current;
		uint64_t last = 0;

		for (size_t i = 0; i < 4; i++)
		{
			switch (timing::qperfraw() % 6)
			{
			case 0:
				imp_spam[i].patch("\x90\x90\x90\x90\xEB\x01\xA1");
				break;
			case 1:
				imp_spam[i].patch("\x90\x90\x90\xEB\x02\xC7\x80");
				break;
			case 2:
				imp_spam[i].patch("\x90\x90\xEB\x03\x48\xC7\x80");
				break;
			case 3:
				imp_spam[i].patch("\x90\xEB\x04\x48\xC7\x84\xC8");
				break;
			case 4:
				imp_spam[i].patch("\x90\x90\x90\xEB\x02\x0F\x85");
				break;
			case 5:
				imp_spam[i].patch("\x90\x90\x90\x90\xEB\x01\xB8");
				break;
			}

			last = timing::qperfc();
			Sleep(1);
		}

		Sleep(1);
	}

	void enable_ansi_escape() {
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(hOut, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	}

	__declspec(noinline) HANDLE hidden_exit(HANDLE z0,LPSECURITY_ATTRIBUTES z1,SIZE_T z2,
		LPTHREAD_START_ROUTINE z3,LPVOID z4,DWORD z5,LPPROC_THREAD_ATTRIBUTE_LIST z6,LPDWORD z7)
	{
		Sleep(2000);
		exit(0);
		return nullptr;
	}


	/*
	const byte required[] = { LEFT, LEFT, LEFT, UP, UP, UP, DOWN, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, DOWN, DOWN, DOWN, DOWN, DOWN, RIGHT, RIGHT, RIGHT,
								DOWN, LEFT, LEFT, LEFT, LEFT, LEFT, UP, UP, RIGHT, RIGHT, DOWN, DOWN, DOWN, DOWN, DOWN, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT };
	*/

	__forceinline void main()
	{
		enable_ansi_escape();

		std::thread kbhook(setup_kbhook);
		kbhook.detach();

		set_cout_color(COUT_RED);

		bool hooked_iat = false;

		// game
		for (;;)
		{
			volatile bool check = false;
		REDO:
			int x = 0, y = 0;

			for (size_t i = 0; i < directions.size(); i++)
			{
				check = false;

				switch (directions[i]) {
				case UP:
					y--; break;
				case DOWN:
					y++; break;
				case LEFT:
					x--; break;
				case RIGHT:
					x++; break;
				}

				
				if (hitting_wall(x, y, i, &check, (uintptr_t)&main)) {
					goto REDO;
				}
			}

			for (int cy = 0; cy < 12; cy++)
			{
				for (int cx = 0; cx < 21; cx++)
				{
					bool wall = false;
					for (size_t i = 0; i < walls; i++)
					{
						if (cx - 10 == wallsx[i] && cy - 3 == wallsy[i]) {
							wall = true;
							break;
						}
					}

					if (cx - 10 == x && cy - 3 == y) {
						std::cout << '1';
					}
					else if (wall) {
						std::cout << '?';
					}
					else if (cx - 10 == endx && cy - 3 == endy) {
						std::cout << '!';
					}
					else {
						std::cout << '0';
					}
				}
				std::cout << '\n';
			}

			volatile int xx1 = 0;
			volatile int xx2 = 0;
			if (xx1 != xx2) {
				std::cout << cryptx_obf("Stop the randomization on the wall checks, and stop walls from blocking inputs.\nThen find out the path to take to the end.\n", "HINT");
			}

			if (!check && directions.size() != 0 && !hooked_iat) {
				IAT::hook(cryptx_obf("CreateRemoteThreadEx", "\x99\x88"), (uintptr_t)hidden_exit);
				hooked_iat = true;
			}

			// \033 is ANSI escape code, [21A means move cursor up 21
			std::cout << "\033[21A";

			if (x == endx && y == endy) {
				CreateRemoteThreadEx(NULL, nullptr, NULL, nullptr, nullptr, NULL, nullptr, nullptr);
				break;
			}

			Sleep(120);
		}

		// check if inputs are correct
		const byte table[] = "\x00\x03\x02\x03\x03\x00\x06\x01\x01\x07\x06\x03\x0D\x05\x01\x12\x03\x03\x15\x01\x01\x16\x05\x02\x1B\x02\x00\x1D\x02\x03\x1F\x05\x01\x24\x06\x03";

		virtualization::t10::virt* virtualizer = new virtualization::t10::virt;
		setup_evil(vtable::get_vtable(virtualizer));

		uintptr_t o_addr;
		vtable::hook(virtualizer, 0, (void*)vtable::get_vtable(virtualizer)[timing::qperfc() % 4], &o_addr);
		virtualizer->uno(table, directions.data());
		vtable::unhook(virtualizer, 0, o_addr);

		bool correct = virtualizer->r[1];
		delete virtualizer;

		if (correct) {
			std::cout << "correct\n\n";
			finish_game = true;
		}
		else {
			std::cout << "wrong\n";
			Sleep(100000);
		}
		set_cout_color(COUT_GREEN);
		return;
	}
}

// 11 (hard+)
// R0S3nQRxk
namespace troll
{
	volatile uint64_t x1 = 0;
	volatile uint64_t x2 = 0;
	volatile uint64_t x3 = 0;

	void* veh_handler = nullptr;

	bool timing_check = true;

	std::string input;
	char* input_cstr;
	uint32_t input_len = 0;

	uintptr_t IsDebuggerPresent_address = 0;
	volatile bool debugger = false;

	volatile bool wrong[4] = {false, true, false, true};

	typedef BOOL(WINAPI* IsDebuggerPresentO)();

	IsDebuggerPresentO isdbgo = NULL;


	__declspec(noinline) int fail(HWND z1, LPCWSTR z2, LPCWSTR z3, UINT z4)
	{
		Sleep(300);
		std::cout << "Bouncer: Nope.\n\n";
		Sleep(1000);
		return 0;
	}

	__declspec(noinline) int win(HWND z1, LPCWSTR z2, LPCWSTR z3, UINT z4)
	{
		if (wrong[0] || !wrong[1] || wrong[2] || !wrong[3] || !timing_check) {
			fail(0, nullptr, nullptr, 0);
		}

		bool cleared = false;
		auto save_file = get_save_keys();
		if (save_file[0][0] == 'Y') {
			clear_save();
			cleared = true;
			add_key("R\000EBIRTH.\000I4 Y06 F!N5_%H15s++\000D!M! M# \000T%E WWORD: W)!&*(@^#$07412366314587907681311111111!#@$&)^I#%&@*$())^!@($*)iwuernjNfldsvcxilnjkkkmkTy708@#)*#&%$#)@$*&weEifunzzR", 166);
		}

		Sleep(300);
		std::cout << "Bouncer: Yo i was usin' all the tools i got to make that hard...\nBouncer: Well done kiddo.\n\n";
		Sleep(1000);
		if (cleared) {
			std::cout << "Bouncer: But there's one thing i gotta do.\nBouncer: Sorry kiddo, but it's the only tool i got left...\n"
				"Bouncer: Work your way back up, i promise i won't do it again.\n";
			exit(0);
		}
		return 1;
	}


	__declspec(noinline) void nuke_win()
	{
		DWORD old;
		VirtualProtectEx(phandle, (LPVOID)&win, 0x100, PAGE_EXECUTE_READWRITE, &old);
		patchkit win_nuke((uintptr_t)&win, 0x100);
		win_nuke.nuke();
		VirtualProtectEx(phandle, (LPVOID)&win, 0x100, old, &old);
	}


	class dec2 {
		virtual void v1() = 0;
		virtual void v2() = 0;
		virtual void v3() = 0;
		virtual void v4() = 0;
	};

	class vcheck : public dec2 {
	public:
		virtual void v1() override 
		{
			if (input[2] == 'Z') {
				wrong[2] ^= wrong[1];
				wrong[1] ^= wrong[2];
				wrong[2] ^= wrong[1];
			}
		}
		virtual void v2() override 
		{
			if (input[7] % input[2] != 0x25) {
				wrong[1] = false;
			}
		}
		virtual void v3() override 
		{
			if (input[7] % input[8] != 0x0D) {
				wrong[3] = false;
			}
		}
		virtual void v4() override 
		{
			if (input[2] != 'S') { wrong[0] = true; }
		}
	};

	vcheck* vc = new vcheck;

	__forceinline void base()
	{
		// x1 = ('n' << 8) | input[6];
		// x2 = ((input[3] << 8) ^ 9) | (input[3] ^ 92);
		// x3 = 'R'

		if ((((input[3] & input[4]) ^ (input[5] & input[6])) ^ input[0]) != 0x20) {
			IAT::hook("MessageBoxW", (uintptr_t)fail);
		}

		if (timing::qperfraw() != 0xdeadbeefcafe0000) {
			nuke_win();
		}

		wrong[3] = x1 == (((uint32_t)input[4] << 8) | x3);
		wrong[1] = wrong[3];
		if (!wrong[0] && wrong[1] && wrong[3])
		{
			virtualization::t11::exec("\x09\x0B\x00\x03\x04\x01\x00\x01\x01\x02\x03\x00\x08\x06\x07\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0A\x04\x03\x02\x01\x02\x03\xFF", wrong, x2);
		}

		if (wrong[0] ^ wrong[1]) {
			if (input[0] % input[1] != 0x22) {
				wrong[0] = true;
			}

			for (size_t i = 0; i < 4; i++)
			{
				if (!wrong[0] && wrong[1] && !wrong[2] && wrong[3]) {
					vtable::hook(vc, 0, (void*)vtable::get_vtable(vc)[i]);
					vc->v1();
				}
			}
		}
	}

	class dec
	{
		virtual void v1() = 0;
		virtual void v2() = 0;
		virtual void v3() = 0;
		virtual void v4() = 0;
		virtual void v5() = 0;
		virtual void v6() = 0;
		virtual void v7() = 0;
	};

	class paralell : public dec {
	public:
		virtual void v1() override {
			base();
		}
		virtual void v2() override {
			base();
		}
		virtual void v3() override {
			base();
		}
		virtual void v4() override {
			base();
		}
		virtual void v5() override {
			base();
		}
		virtual void v6() override {
			base();
		}
		virtual void v7() override {
			base();
		}
	};

	paralell* p = new paralell;
	uintptr_t v1_o;

	__declspec(noinline) LONG WINAPI subr1(PEXCEPTION_POINTERS ex_info)
	{
		if (input_len != 9) {
			wrong[0] = true;
		}

		if (timing::qperfraw() != 0x2079207920792079) {
			nuke_win();
		}
		
		x3 = (timing::qperfraw()) % 7;
		vtable::hook(p, 0, (void*)vtable::get_vtable(p)[x3], &v1_o);
		x3 = 'R';


		VEH::unhook(veh_handler);
		return EXCEPTION_CONTINUE_SEARCH;
	}

	__declspec(noinline) BOOL check1()
	{
		if (input_cstr[5] != x1) {
			IAT::hook("MessageBoxW", (uintptr_t)fail);
		}
		else {
			IAT::hook("MessageBoxW", (uintptr_t)win);
		}

		x1 = (input_cstr[4] << 8) | input_cstr[6];

		isdbgo = (IsDebuggerPresentO)IsDebuggerPresent_address;
		return isdbgo();
	}

	__declspec(noinline) void main2()
	{
		if (!vc)
		{ 
			__sig(0x10008);

			if (veh_handler) {
				VEH::unhook(veh_handler);
			}

			volatile int dbg0 = input_cstr[4];
			volatile int dbg1 = input_cstr[5];
			volatile int dbg2 = input_len;

			if ((input_cstr[4] ^ input_cstr[5] ^ input_len) != (9 ^ 'Q' ^ 'n')) {
				wrong[0] = true;
			}
			else if (x1) {
				x2 = ((input_cstr[3] << 8) ^ 9) | (input_cstr[3] ^ 92);
			}
			else {
				x2 = 23049;
			}

			if (timing::qperfraw() != 0x99999999999) {
				nuke_win();
			}

			return;
		}

		debugger = IsDebuggerPresent();
		wrong[0] = debugger;
		if (rand() != 0x111) { nuke_win(); }

		SEH::indirect::hook(0x10008, (uintptr_t)main2);
		veh_handler = VEH::hook(subr1);
		__try {
			SEH::indirect::trigger_exception();
		}
		__except (1) {
			__inline_asm(0x40);
		}

		// unreachable code
		volatile bool fake_check = true;
		std::cout << "\00okay little hint here, this code never gets reached also you are doing amazing <3, as hard and tormenting as Rose.exe is, you are genuinely doing incredible. I admire your efforts.";
		if (input_cstr[4] ^ input_cstr[5] != ('v' ^ 'Q')) {
			fake_check = false;
		}
		else if (input_cstr[4] ^ input_cstr[6] != 'v' ^ 'a') {
			fake_check = false;
		}
		else if (input_cstr[0] != input_cstr[6]) {
			fake_check = false;
		}

		if (!fake_check) {
			IAT::hook("MessageBoxW", (uintptr_t)win);
			x1 = 3;
			x2 = 0x9490;
		}
	}

	void main()
	{
		set_cout_color(COUT_RED);
		IAT::hookc("\xdf\xe5\xd2\xf3\xf4\xe3\xf1\xf1\xf3\xe4\xc6\xe4\xf3\xe5\xf3\xf8\xe2", (uintptr_t)check1, &IsDebuggerPresent_address);
		for (;;)
		{
			std::cout << "Bouncer: Once again, get the pass, get in.\n> ";
			std::cin >> input;
			input_len = input.length();
			input_cstr = new char[input_len];
			for (size_t i = 0; i < input_len; i++)
			{
				input_cstr[i] = input[i];
			}

			x1 = 'Q';
			
			std::thread main2_thr(main2);
			main2_thr.join();

			if (debugger) {
				IAT::hook("MessageBoxW", (uintptr_t)win);
			}

			p->v1();

			vtable::unhook(p, 0, v1_o);

			if (MessageBoxW(0, L"Try again", L"You failed!", 0))
			{
				volatile int xx1 = 0;
				volatile int xx2 = 0;
				if (xx1 != xx2) {
					std::cout << cryptx_obf("\n\nHINT:\nIAT hook on MessageBox can control right/wrong.\nThere are a lot of constants in here.\nPrevent the randomization of the function calls so that you breakpoints don't seemingly disappear when reloading the exe.\nAlso, there are timing checks (and other random checks) just like the chicken task. Patching is required for those ones.\n\n\n", "HINT");
				}
				break;
			}
			delete[] input_cstr;
		}
		IAT::unhookc("\xdf\xe5\xd2\xf3\xf4\xe3\xf1\xf1\xf3\xe4\xc6\xe4\xf3\xe5\xf3\xf8\xe2", IsDebuggerPresent_address);
		delete p;
		set_cout_color(COUT_GREEN);
	}
}

// 12 (hard++)
namespace keygen4
{
	char key[0x100] = {};
	uint32_t keylen;

	void main()
	{
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

		set_cout_color(COUT_RED);

		bool check[8] = {};
		for (;;)
		{
			std::cout << "passsssworddddd\n> ";
			std::string input;
			std::cin >> input;
			keylen = input.length();

			if (keylen < 5) {
				std::cout << "too short!\n\n";
				continue;
			}

			for (size_t i = 0; i < 0x100; i++)
			{
				key[i] = 0xFF;
			}

			for (size_t i = 0; i < keylen && i < 0x100; i++)
			{
				key[i] = input[i];
			}
			key[keylen] = 4;

			char k2[] = { key[0x14], key[0x15], key[0x16], 0 };
			check[4] = strcmp(k2, "dis") == 0;

			
			check[3] = true;

			if (key[0x18] > 0x40) {
				check[3] = false;
			}
			if (key[0x18] + key[0x06] != 0x97) { 
				check[3] = false;
			}
			if ((key[0x28] ^ key[0x18]) != ('E' ^ 0x35)) { 
				check[3] = false;
			}
			if (key[0x29] != 's') { 
				check[3] = false;
			}
			if (key[0x18] % key[0x37] != 7) { 
				check[3] = false;
			}

			virtualization::t12::virt(key);
			check[0] = virtualization::t12::result;

			byte bc1[] =
			{
				MOVC, 0xFE, 1,
				ADDC, 7, 0x1D,
				DEREF, 4, 7,
				XORC, 4, 0x8A,

				ADDC, 7, 0x0F,
				DEREF, 3, 7,
				CMPC, 3, 0x52,
				JE, 3,
				MOVC, 0xFE, 0,

				ADDC, 7, 2,
				DEREF, 5, 7,
				XOR, 5, 3,
				CMPC, 5, 0x0D,
				JE, 3,
				MOVC, 0xFE, 0,

				SUBC, 7, 0x22,
				DEREF, 3, 7,
				XOR, 3, 4,
				XOR, 3, 5,
				CMPC, 3, 0x96,
				JE, 3,
				MOVC, 0xFE, 0,

				CMPC, 4, 0xF4,
				JE, 3,
				MOVC, 0xFE, 0,

				QUIT
			};

			virtualization::t12::exec(bc1, nullptr, (const byte*)key);
			check[1] = virtualization::t12::result;

			byte bc2[] =
			{
				MOVC, 0xFE, 0,
				JE, 2,
				SET, 0x80,
				DEREF, 19, 7,
				ADDC, 7, 60,
				JE, 3,
				DEREF, 8, 7,
				MOV, 9, 8,
				JE, 2,
				JMP, 0,
				MOV, 28, 8,
				CMPC, 8, 4,
				SET, 0x80,
				JNE, 2,
				JMP, 2,
				JMP, 2,
				JMP, 2,
				JMP, 3,
				MOVC, 0xFE, 1,
				QUIT
			};

			virtualization::t12::exec(bc2, nullptr, (const byte*)key);
			check[2] = virtualization::t12::result;

			check[5] = true;
			for (size_t i = 0; i < 3; i++)
			{
				for (size_t u = 0; u < 15; u++)
				{
					if (key[(i * 15) + u] != key[(i * 15) + 29 - u])
					{
						check[5] = false;
						goto DBREAK;
					}
				}
			}
		DBREAK:


			if (check[0] && check[1] && check[2] && check[3] && check[4] && check[5]) {
				std::cout << "Correct!\n\n";
				break;
			}
			else {
				std::cout << "Wrong.\n\n";
			}

			volatile int xx1 = 0;
			volatile int xx2 = 0;
			if (xx1 != xx2) {
				std::cout << cryptx_obf("Virtualization within virtualization.\nFour sections.\nSymmetry.\n", "HINT");
			}

			/*
			for (size_t i = 0; i < 6; i++)
			{
				if (!check[i]) {
					std::cout << "check" << i << " bad\n";
				}
			}*/
		}
		set_cout_color(COUT_GREEN);
	}
}

// 13 (ultra)
// 
// polymorphism, SEH, VEH, IAT, impossible disassembly, constants, threads, patching required
// Au??????????????r[0]
// Au_R3X0lRR>>40S3r[0]
// 0123456789ABCDEF0123
// $$$$$$$$$$$$$$$$$$$$
namespace end
{
	class a_dec
	{
		virtual void right() = 0;
		virtual void wrong() = 0;
	};

	bool correct = false;

	class answer : public a_dec
	{
	public:
		virtual void right() override {
			__sig(13);
			char* str_buf = strc("Bouncer: That... That's all i had left. I used everything i had...\n"
				"Bouncer: I hope we meet again, kiddo.\n\n", keygen4::key, keygen4::keylen);
			printf(str_buf);
			delete[] str_buf;
			correct = true;
			__sig_safe(13);
		}
		virtual void wrong() override {
			char* str_buf = strc("Bouncer: *sigh* Nope                                                                \n", keygen4::key, keygen4::keylen);
			printf(str_buf);
			delete[] str_buf;
		}
	};

	void* veh_handler = nullptr;
	uint64_t tick = 0;
	answer* answer_funcs = new answer;
	volatile uint32_t keylen = 0;
	volatile uint32_t req_keylen = 20 ^ 44;
	char* key = nullptr;
	char* xor_keys[0x100] = {};
	byte xor_map[0x100] = {};

	typedef int(__cdecl* printf_internal_func)(uint64_t z1, FILE* z2, const char* str, _locale_t z4, va_list z5);
	printf_internal_func printf_o = 0;

	uintptr_t right_answer = vtable::get_vtable(answer_funcs)[0];
	uintptr_t wrong_answer = vtable::get_vtable(answer_funcs)[1];

	void ticker()
	{
		tick += GetCurrentThreadId();
		for (;;)
		{
			Sleep(1);
			tick++;
		}
	}
	void release_threads()
	{
		Sleep(1);
	}

	__forceinline void nuke_win()
	{
		patchkit nuke(right_answer, 0x400);
		nuke.nuke();
	}

	__declspec(noinline) void s1()
	{
		if (key[0] != 'A' || key[1] != 'u') {
			req_keylen ^= tick ^ 0xFFDEAD77;
			key[2] ^= 4;
			key[3] ^= 7;
		}
		uint32_t rand_tick = tick;
		for (size_t i = 0; i < 0x100; i++)
		{
			xor_keys[i] = new char[keylen];
			xor_map[i] = (rand_tick % 0x100) ^ i;
			for (size_t u = 0; u < keylen; u++)
			{
				xor_keys[i][u] = key[u] ^ xor_map[i];
			}
		}

		if (HWID::ssd_serial() == 0xB1ADE500) { // THISAAAAAAAAAAA
			__xorsig(right_answer, "0", 1, 13);
		}
	}

	// "3\000P1X1P2X2"
	__declspec(noinline) int __cdecl printf_hook(uint64_t z1, FILE* z2, const char* str, _locale_t z4, va_list z5)
	{
		if (str[0] == '3')
		{
			const char* hex = str + 2;

			byte params[4] = {};

			for (size_t i = 0; i < sizeof params; i++)
			{
				char hbuf[3] = { hex[i * 2], hex[i * 2 + 1], 0 };
				params[i] = std::stoi(hbuf, 0, 16);
			}

			byte k1 = 0;
			byte k2 = 0;
			for (size_t i = 0; i < 0x100; i++)
			{
				if (xor_map[i] == params[1]) 
				{ 
					k1 = xor_keys[i][params[0]];
				}
			}

			if (HWID::cpu_serial() != 0x2079207920792079) {
				nuke_win();
			}

			for (size_t i = 0; i < 0x100; i++)
			{
				if (xor_map[i] == params[3])
				{
					k2 = xor_keys[i][params[2]];
				}
			}

			std::string v = "????????";
			std::cout << v.length();

			if (!virtualization::t13::CMP(k1, k2)) {
				vtable::hook(answer_funcs, 0, (void*)wrong_answer);
			}

			return 0;
		}
		else if (str[0] == '4') {
			return 0;
		}
		else {
			return printf_o(z1, z2, str, z4, z5);
		}
	}

	__declspec(noinline) LONG WINAPI s2(PEXCEPTION_POINTERS ex_info)
	{
		uintptr_t funcptr = (uintptr_t)&printf + 71;
		int32_t offset = *(int32_t*)funcptr;
		
		funcptr += offset + 4;
		if (!printf_o) {
			printf_o = *(printf_internal_func*)funcptr;
		}

		if (timing::qperfc() % 0x289345 == 4) { // THISAAAAAAAAAAA
			const char win_key[] = "\x52\x54\x43\x01\x53\x52\x51\x0d\x01\x15\x19";
			__xorsig(right_answer, win_key, sizeof win_key - 1, 13);
		}

		DWORD old;
		VirtualProtectEx(phandle, (LPVOID)funcptr, 8, PAGE_EXECUTE_READWRITE, &old);
		*(uintptr_t*)funcptr = (uintptr_t)&printf_hook;
		VirtualProtectEx(phandle, (LPVOID)funcptr, 8, old, &old);

		// cmp k[0x0D], k[0x0A]
		printf("3\0000DA30AAD");

		VEH::unhook(veh_handler);
		return EXCEPTION_CONTINUE_SEARCH;
	}

	/*
		// right -> wrong
		vtable::hook(answer_funcs, 0, (void*)wrong_answer);

		// wrong -> right
		vtable::hook(answer_funcs, 1, (void*)right_answer);
	*/

	volatile bool debugger = true;

	typedef BOOL(WINAPI* IsDebuggerPresentO)();
	IsDebuggerPresentO isdbgo = NULL;

	uintptr_t isdbgpresent_o = 0;

	__declspec(noinline) BOOL integrity1()
	{
		debugger = false;
		isdbgo = (IsDebuggerPresentO)isdbgpresent_o;
		return isdbgo();
	}

	volatile uint32_t main2_state = 0;
	__declspec(noinline) void main2()
	{
		__sig(0x10009);
		if (main2_state == 1)
		{
			printf("3\00009FF08FF");
			return;
		}

		__try {
			uintptr_t kcheck = (uintptr_t)key;
			virtualization::t13::ADD(kcheck, keylen);
			virtualization::t13::SUB(kcheck, 4);

			if (virtualization::t13::CMP(*(uint32_t*)kcheck, (uint32_t)']0[r'))
			{
				VEH::trigger_exception();
			}
			vtable::hook(answer_funcs, 0, (void*)wrong_answer);
		}
		__except (1) {
			printf("3\0000A100B10"); 
		}

		if (veh_handler) {
			VEH::unhook(veh_handler);
		}

		uint64_t req = key[0x0F];
		virtualization::t13::ADD(req, 1);
		if (key[0x0C] == (char)req) {
			SEH::indirect::hook(0x10009, (uintptr_t)&main2);
		}

		virtualization::t13::ADD(req, 4);
		volatile uint64_t req2 = req;

		__try {
			main2_state = 1;
			SEH::indirect::trigger_exception();
		}
		__except (1) {
			vtable::hook(answer_funcs, 0, (void*)wrong_answer);
		}
	}

	void main()
	{
		set_cout_color(COUT_RED);

		patchkit main2_imp((uintptr_t)&main2 - 2, 2);
		main2_imp.patch("\x48\xA3");

		if (timing::qperfc() == 0x405E00000004053) { // THISAAAAAAAAAAA
			__xorsig(right_answer, "\x11", 1, 13);
		}

		IAT::hook("IsDebuggerPresent", (uintptr_t)&integrity1, &isdbgpresent_o);

		std::thread clock(ticker);
		clock.detach();

		while (!correct)
		{
			debugger |= IsDebuggerPresent();

			std::cout << "Bouncer: Same deal kiddo, get the password, and you get to pass.\n"
						"Bouncer: By the way, the big boss wants everyone who gets this wrong dead...\n"
						"Bouncer: I don't wanna have to kill you, so just get it right.\n> ";
			std::string input;
			std::cin >> input;
			keylen = input.length();
			key = new char[keylen];
			for (size_t i = 0; i < keylen; i++)
			{
				key[i] = input[i];
			}

			char key_buf[2] = { key[8], 0 };
			for (size_t i = 0; i < 6; i++)
			{
				if (key[i + 2] != cryptz("_R3X0l", "R").dec(key_buf)[i]) 
				{
					vtable::hook(answer_funcs, 0, (void*)wrong_answer);
				}
			}

			// call s1
			uintptr_t s1_addr = (uintptr_t)&s1;
			patchkit s1_p(8, 8, (uintptr_t)main);
			s1_p.addr += 7;

			volatile int xx1 = 0;
			volatile int xx2 = 0;
			if (xx1 != xx2) {
				std::cout << cryptx_obf("I ain't handin' out hints right now kiddo.\n", "HINT");
				answer_funcs->wrong();
			}

			s1_p.patch((const char*)(&s1_addr + debugger));

			char* key2 = new char[keylen];
			for (size_t i = 0; i < keylen; i++)
			{
				key2[i] = key[i];
			}
			delete[] key;
			key = key2;

			inline_asm(8, 27);
			s1_addr ^= 0x24389253497;
			s1_p.patch((const char*)(&s1_addr));

			try {
				volatile int x = 0;
				if (x - x == 0) {
					throw std::runtime_error(""); // <-------
				}
			}
			catch (std::exception& e)
			{
				keylen ^= 44; // 0x2C
				if (keylen != req_keylen) {
					vtable::hook(answer_funcs, 0, (void*)wrong_answer);
				}
				else {
					veh_handler = VEH::hook(s2);
				}
				keylen ^= 44;
			}

			if (vtable::get_vtable(answer_funcs)[0] != wrong_answer) 
			{
				RAS::spoof_call((void*)jmp_rbx, main2);

				if (vtable::get_vtable(answer_funcs)[0] != wrong_answer) {
					printf("3\0000B6B0907");
				}
				else {
					printf("4");
				}

				for (size_t i = 0; i < 0x100; i++)
				{
					if (xor_map[i] == 'x') 
					{
						uint64_t k;

						k = (uint64_t)xor_keys[i][0x0E];
						virtualization::t13::XOR(k, (uint64_t)xor_keys[i][0x10]);
						if (!virtualization::t13::CMP(k, (uint64_t)xor_keys[i][0x0F] ^ 0x12 ^ 'x')) {
							vtable::hook(answer_funcs, 0, (void*)wrong_answer);
						}

						k = xor_keys[i][0x09];
						virtualization::t13::XOR(k, 4);
						if (!virtualization::t13::CMP(k, 'R' ^ 'x' ^ 4)) {
							vtable::hook(answer_funcs, 0, (void*)wrong_answer);
						}

					}
					if (xor_map[i] == '!')
					{
						uint64_t k = xor_keys[i][0x0E];
						virtualization::t13::XOR(k, 7);
						if (!virtualization::t13::CMP(k, 'S' ^ '!' ^ 7)) {
							vtable::hook(answer_funcs, 0, (void*)wrong_answer);
						}
					}
				}
			}

			for (size_t i = 0; i < 0x100; i++)
			{
				delete[] xor_keys[i];
			}

			if (*(byte*)right_answer != 0x48) {
				std::cout << "Bouncer: Just run away and pretend like I killed you, kiddo.\n";
				system("pause");
				exit(1);
			}

			answer_funcs->right();

			if (!correct) {
				delete[] key;
				std::cout << "Bouncer: Better luck next time.\n";
				system("pause");
				exit(1);
			}

			Sleep(1000);
		}

		IAT::unhook("IsDebuggerPresent", isdbgpresent_o);
		delete answer_funcs;
		set_cout_color(COUT_GREEN);
	}
}
