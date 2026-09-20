#include <windows.h>
#include <winternl.h>
#include <vector>

#include "print.h"
#include "mem.h"
#include "cout_color.h"
#include "save.h"
#include "name.h"

#include "strcryptx.hpp"
#include "hashing.hpp"
#include "sigs.hpp"

#include "inline_asm.h"
#include "obfuscation.h"
#include "strc.h"
#include "effects.h"

#include "Tasks.h"

#include <iostream>

// used to speed up the process of testing
constexpr bool DBG = false;
constexpr bool bypass = false;


// used to test individual features during development
__declspec(noinline) void main2() 
{
	SEH::indirect::hook(0x493, (uintptr_t)main2);
	__sig(0x493);
	std::cout << "yes\n";
	__try {
		SEH::indirect::trigger_exception();
	}
	__except (1) {
		std::cout << "no\n";
	}

	Sleep(1000000);
}


int main()
{
	//main2();

	// for indication that rosencryptor has been run or not so that i don't accidentally forget and waste time
	// this is patched out in rosencryptor so that if it can't find sig 0xFFFF then it knows it's already been encrypted
	__sig(0xFFFF);
	__nop();

	inline_asm(1, 3);

	if (!DBG && !bypass) {
		// check if rosencryptor has been ran
		uintptr_t stoopid_addr = find_signature((uintptr_t)main, 0xFFFF, 1, 0x1000);

		// rosencryptor will nuke the sig, so that it can't be found, therefore the game will run as usual and not do this
		if (stoopid_addr != 0) {
			print("Ah.|\nRight.|\nI-|\nI have made a small mistake.|\nJust give me a second.|||");
			return 0xDEAD0000;
		}
	}

	// horrible brute-forced non-scaling code all throughout here
	// the reason was because i just wanted it to *work* and be *simple* and didn't care about the scaling at the time
	// if i did this again i would make something much more scalable and less brute-forced
	// theres also a lot of magic numbers...
	unsigned char* key = new unsigned char[0x1000];

	BYTE run_check[3] = {};
	BYTE run_check_first_game[10];
	char keygen1[30] = {};
	uint32_t keygen1_len = 0;
	char keygen2[30] = {};
	uint32_t keygen2_len = 0;
	char password1[30] = {};
	uint32_t password1_len = 0;
	volatile uint64_t chicken_result = 0;
	char keygen3[30] = {};

	set_cout_color(COUT_GREEN);

	std::vector<std::vector<char>> save_file = get_save_keys();

	// to easily set where i want to go to without using save file manipulation
	if (DBG) { goto DBG_JMP; }
DBG_JMP:

	if (save_file.size() <= 0) {
		print("[A step into the unknown]\n\nWelcome, welcome.|\nBetter late then never.||\nI'm your supervisor and will tell you the next test whenever you've completed the previous one.|\n"
			"I'll get you started on an easy test, and slowly get harder.|\n"
			"This test will be easy, you need to "
			"walk before you can run, and run, before you can fly.|||\n\n"
			"Patch the loop so that it stops looping, and so that you can continue.\n\n");
		first_stage();
		add_key("You can do it.", 20 - 1);
	}

	if (save_file.size() <= 1) {
		print("[The call]\n\nGood start.|\nThe second test is to change the print function used in this program.|\n"
			"Change it so that the Sleep when printing a certain character is 0x10 ms instead of 0x12C ms.|\n"
			"Do not change anything else.|\n"
			"Take as much time as you need.|\nI'll be waiting.\n\n\n");

		for (;;)
		{
			if (patch_print_check(key)) {
				break;
			}

			Sleep(500);
		}
	}
	else 
	{
		for (size_t i = 0; i < save_file[1].size(); i++)
		{
			key[i] = save_file[1][i];
		}
	}

	__xorsig(main, (const char*)&key[95 - 5], 5, 1);


	__sig(1);
	if (save_file.size() <= 1) {
		add_key((const char*)key, 95);
	}
	if (save_file.size() <= 2)
	{
		print_c("[The bouncer]\n\nWell done.|\nThe third task is to once again patch a loop to continue.\n"
			"However, there is now security around.|\nThe bouncer will make sure people can't get in.\n",
			(const char*)&key[95 - 5], 5, 40);

		first_run_check(main, (char*)run_check);
	}
	else
	{
		for (size_t i = 0; i < save_file[2].size() && i < 3; i++)
		{
			run_check[i] = save_file[2][i];
		}
	}
	__sig(1);

	__xorsig(main, (const char*)run_check, 3, 2);

	__sig(2);
	if (save_file.size() <= 2) {
		add_key((const char*)run_check, 3);
	}
	if (save_file.size() <= 3)
	{
		print_c("[Adventures]\n\nWhat a playful bouncer.|\nYou go into the house and decide to leave after you see a couple idiots that you know of in there, so you just leave and do something more important.||\n\n"
			"I've realised that I haven't told you any information on what will happen when you finish all tasks.|\n"
			"Or if you don't.|\n"
			"Hm.|||\n\n"
			"Now is not the time.\n\n"
			"Your fourth task is to beat the boss in this little game to save the person or whatever the story is.\n\n\n",
			(const char*)run_check, 3, 40);

		first_game(run_check_first_game);

		auto run_check_hash = ch_a("\x1e\x73\x73\x03\xad\xff\x01\x62\x62\xe7");
		BYTE runtime_hash[10] = {};
		memcpy(runtime_hash, run_check_first_game, 10);
		ch_r(runtime_hash, 10);

		if (!ch_cmp(runtime_hash, run_check_hash.data, 10)) {
			print("This is bad.|\nYou probably skipped a run check, which is caused by unnessecarily skipping code.");
			print("The next part will not decrypt properly.|\nI will close the program, you reopen it.||||\n");
			return 0xFA11;
		}
	}
	if (save_file.size() > 3)
	{
		for (size_t i = 0; i < save_file[3].size() && i < 10; i++)
		{
			run_check_first_game[i] = save_file[3][i];
		}
	}
	__sig(2);

	__xorsig(main, (const char*)run_check_first_game, 10, 3);

	__sig(3);
	if (save_file.size() <= 3) {
		add_key((const char*)run_check_first_game, 10);
	}
	if (save_file.size() <= 4) 
	{
		if (!DBG) {
			print_c("[The first encounter to a password]\n\nThen you save the traveller and live happily ever after.||\n"
				"Isn't that nice.|\n\nYour next task is to generate a key which will suit the key check algorithm in the code.|\n"
				"Make sure that you use the closest letters to 'a' as possible for the key.|\n"
				"Also use the least amount of letters possible.|\nPatching won't work here.||\n"
				"Oh and one more thing...|\nThe amount of code is little, but it doesn't mean it's easy.|\n"
				"The short stick is harder to break than the long stick.|\n\n",
				(const char*)run_check_first_game, 10, 40);
		}

		for (;;) 
		{
			set_cout_color(COUT_RED);
			std::cout << "Enter the key:\n> ";
			char input_key[30] = {};
			std::cin >> input_key;

			for (size_t i = 0; i < 30; i++)
			{
				if (input_key[i] == 0) {
					keygen1_len = i;
					break;
				}
			}

			memcpy(keygen1, input_key, keygen1_len);

			// VOLATILE CLUTCHED UP THE OPTIIII
			volatile bool valid = first_keygen(input_key, keygen1_len);
			if (!valid) { std::cout << "Wrong.\n"; continue; }

			auto keygen1_c = ch_a("bfjnrv");
			char keygen1_r[30];
			memcpy(keygen1_r, keygen1, keygen1_len);
			ch_r((unsigned char*)keygen1_r, keygen1_len);
			if (ch_cmp((BYTE*)keygen1_r, (BYTE*)keygen1_c.data, keygen1_len))
			{
				std::cout << "You may pass.\n\n";
				set_cout_color(COUT_GREEN);
				break;
			}
			else {
				std::cout << "Not quite. Make sure that the key's letters are closest to 'a' as possible.\n";
			}
		}
	}
	if (save_file.size() > 4) {
		for (size_t i = 0; i < save_file[4].size() && i < 12; i++)
		{
			keygen1[i] = save_file[4][i];
		}

		for (size_t i = 0; i < 30; i++)
		{
			if (keygen1[i] == 0) {
				keygen1_len = i;
				break;
			}
		}
	}
	__sig(3);

	__xorsig(main, keygen1, keygen1_len, 4);

	__sig(4);
	if (save_file.size() <= 4) {
		add_key(keygen1, keygen1_len);
	}
	if (save_file.size() <= 5) {
		if (!DBG) {
			print_c("[The bouncer comes again]\n\nGood job.|\nNow it's starting to become a real challenge.|\n"
				"Well hopefully you've got the hang of it and it's time for a harder key problem.|\n"
				"The bouncer is back and he won't be so lenient this time.|\nHe wants the password.|\n"
				"No way around it.|\n\n"
				"Make sure that the password is as short as possible.|\n\n\n",
				keygen1, keygen1_len, 40);
		}

		set_cout_color(COUT_RED);
		std::cout << "Bouncer: You again? Look kiddo, I know you ain't invited, but you get the password, you in.\n> ";
		for (;;)
		{
			char input_key[30] = {};
			std::cin >> input_key;

			uint32_t kl = 0;
			for (size_t i = 0; i < 30; i++)
			{
				keygen2[i] = input_key[i];
				if (input_key[i] == '\x00') {
					kl = i;
					break;
				}
			}
			if (second_keygen(input_key, kl)) {
				auto hash = ch_a("fdfhjlnljhfdf");
				char hash_r[30];
				memcpy(hash_r, keygen2, 30);
				ch_r((unsigned char*)hash_r, kl);
				if (ch_cmp(hash.data, (BYTE*)hash_r, kl)) {
					keygen2_len = kl;
					std::cout << "Bouncer: I'm proud, kiddo. Next time I'll get a harder challenge goin'.\n\n";
					break;
				}
				else {
					std::cout << "Bouncer: Huh? Nah not it. Dunno what you did there.\n> ";
				}
			}
			else {
				std::cout << "Bouncer: Not it, kid.\n> ";
			}
		}
		set_cout_color(COUT_GREEN);
	}
	if (save_file.size() > 5) {
		keygen2_len = 13;
		for (size_t i = 0; i < save_file[5].size() && i < 13; i++)
		{
			keygen2[i] = save_file[5][i];
		}
	}
	__sig(4);

	__xorsig(main, keygen2, keygen2_len, 5);

	__sig(5);
	if (save_file.size() <= 5) {
		add_key(keygen2, keygen2_len);
	}
	if (save_file.size() <= 6) {
		print_c("[Deception]\n\nYou can hack an obfuscated, simple game.|\nYou can reverse and understand optimized assembly.|\n"
			"But can you adapt when an unexpected problem occurs?|\n"
			"We are going to find out.|\nYour next task is to find the hidden password and input it.|\n\n"
			"Question everything. Reality is only what the mind perceives.\n\n",
			keygen2, keygen2_len, 40);
		
		set_cout_color(COUT_RED);
		for (;;) {
			char* prompt = strc("Password                        \n> ", keygen2, keygen2_len);
			std::cout << prompt;
			delete[] prompt;

			char userinput[30] = {};
			std::cin >> userinput;
			if (imp_disassembly(userinput)) {
				char* right = strc("Correct.                        \n\n", keygen2, keygen2_len);
				std::cout << right;
				delete[] right;

				password1_len = strlen(userinput);
				memcpy(password1, userinput, password1_len);
				set_cout_color(COUT_GREEN);
				break;
			}
			else {
				char* wrong = strc("Wrong.                        \n", keygen2, keygen2_len);
				std::cout << wrong;
				delete[] wrong;
			}
		}
	}
	if (save_file.size() > 6) {
		password1_len = 10;
		for (size_t i = 0; i < password1_len; i++)
		{
			password1[i] = save_file[6][i];
		}
	}
	__sig(5);

	__xorsig(main, password1, password1_len, 6);

	__sig(6);
	if (save_file.size() <= 6) {
		add_key(password1, password1_len);
	}
	if (save_file.size() <= 7) {
		print_c("[The chicken]\n\nImpressive. Very nice.\nHey why did the chicken cross the road?||||\n"
			"Today you're going to find out.\n"
			"Get this chicken to cross the road without getting hit, and he might tell you why.\n\n"
			"Patching is fine, but be careful.\n"
			"He will never tell you why the chicken crossed the road if you patch wrong...\n\n",
			password1, password1_len, 37);

		chicken_result = chicken();
		if (chicken_result != 24599958760) {
			std::cout << cryptx_obf("Actually, no you didn't. Chicken died. You did something wrong.\n", "do it properly");
			Sleep(0xB1A2E);
			exit(0xDEAD);
		}
		set_cout_color(COUT_GREEN);
		std::cout << cryptz("Thank you stranger.\nAh yes, what you wanted was the answer to the age old question:\n"
			"It's a good challenge for a chicken and provides entertainment for their families.\nToodles!\n\n\n", "\xE8\x94\x45\xBA\x05\x00\x00\x00").dec((const char*)&chicken_result);
	}
	if (save_file.size() > 7) {
		for (size_t i = 0; i < 8 && i < save_file[7].size(); i++)
		{
			chicken_result |= ((uint64_t)(uint8_t)save_file[7][i]) << ((i) * 8);
		}
	}
	__sig(6);

	__xorsig(main, (const char*)&chicken_result, 8, 7);

	__sig(7);
	if (save_file.size() <= 7) {
		add_key((const char*)&chicken_result, 8);
	}
	if (save_file.size() <= 8) 
	{
		print_c("[A bouncer and his bytecode]\n\nWhat a dumb chicken. Anyways, congratulations on putting up with him.|\n"
				"Your next task has no SEH, junk code or anti-disassembly techniques, so have a nice break.\n"
				"And right into some real reverse engineering.||\n\n"
				"The bouncer is back and working for a more important club.\n"
				"After this task I have a special present for you.||\n\n"
				"Good luck.|||\nYou are going to need it.\n\n", 
				(const char*)&chicken_result, 8, 37);

		set_cout_color(COUT_RED);
		std::cout << "Bouncer: Ay you again? Haven't seen ya in a while.\n";
		std::cout << "Bouncer: Anyways we got some new tech, let's see if you can handle it\n\n";

		for (;;)
		{
			// D!jP?tP)~
			std::cout << "Bouncer: Gimme the pass\n> ";
			char input[30] = {};
			std::cin >> input;

			virtualization::t9 virt;
			virt.boolean2 = 1;
			virt.rax = (uint64_t)input;

			virt.exec("\x09\x05\x03\x02\x0B\x03\xFF\x6A\x08\x01\xFB\x09\x05\x03\x01\x0B\x03\xFF\x21\x08\x01\xFB\x09\x05\x12\x0B\x03\xFF\x00\x08\x22\xF9\x01\xF8\x08\x1A\x09\x05\x12\x06\x01\x09\x03\x05\x01\x0B\x03\x0B\x02\x0F\x06\x03\x05\x09\x04\x13\x0B\x02\xFE\x08\x01\xFB\x11\x07\xD5\x09\x07\xFF\x09\x08\x01\xFB");

			std::cout << "Bouncer: Hmm lemme think.\n";
			if (virt.boolean2) {
				std::cout << "Bouncer: Wow. Okay. In ya go.\n\n";
				memcpy(keygen3, input, 30);
				break;
			}
			else {
				std::cout << "Bouncer: Nope.\n\n";
			}
		}
		set_cout_color(COUT_GREEN);
	}
	if (save_file.size() > 8) {
		for (size_t i = 0; i < save_file[8].size() && save_file[8][i] != '\n'; i++)
		{
			keygen3[i] = save_file[8][i];
		}
	}
	__sig(7);

	__xorsig(main, keygen3, strlen(keygen3), 8);

	__sig(8);
	if (save_file.size() <= 8) {
		add_key((const char*)keygen3, strlen(keygen3));
	}
	if (save_file.size() <= 9) 
	{
		print_c("[Great labyrinth]\n\nThe bouncer is keeping and eye on your improvement.|\nMake him proud.|\n"
				"Anyways, I promised you a special gift.|\n",
			keygen3, strlen(keygen3), 35);
		cout_c("            .\n        '    .  '\n     '      .      '\n   .         .       .\n            .\n  .         .;        .\n           .':\n  .       .'  :       .\n          :   ;\n   .       \./       .\n           .'\n     .   :;|.;:   .\n        'd '' |'\n         |'  .|\n         |    |\n         |    |\n         :. ' |\n         |    |\n         |    b\n         c    |\n         | . '',\n         |    ')\n         |    :\n         |    |\n         |c   |\n         |    |\n         |    |\n         |    |\n       __|   :|__\n   .-\"\" .|  , |. \"\"-..-\"\".\n  ::.--\"\"|.   |\"\"--.::    :\n  :.   '.;:__.+:.   .:\"__:'\n  : \"-.._\"   \" _..-\" : \"\"\n   \"..   \"\"\"\"\"\"   ..\"\n      \"\"--....--\"\"grp\n\n",
			keygen3, strlen(keygen3));
		std::string name_of_user = get_users_name();

		print_c("You've been working in the dark this whole time, ",
			keygen3, strlen(keygen3), 35);
		Sleep(1000);
		print(name_of_user.c_str(), 400);

		std::cout << '\n';

		// sigh... nobody ever found this easter egg when solving
		// sadly lebron wasn't up for the challenge i guess
		// me: 1
		// lebron: 0
		if (name_of_user == "rose" || name_of_user == "Rose" || name_of_user == "dancer"
			|| name_of_user == "lebron james" || name_of_user == "LaughableTwisty")
		{
			print("Wait a second. ", 35);
			print(name_of_user.c_str(), 35);
			print_c("??|\nIt's really you?|\n"
				"I recognized you but...||\n"
				"I didn't believe my eyes.|\n\n"
				"Well aren't I lucky...|||\n\nThe tasks will still run as usual.|\n\n",
				keygen3, strlen(keygen3), 35);
		}
		else {
			print("||\n\n");
		}

		print_c("Anyways, I hope you like the candle.|\n\n\n"
				"For this one, use arrow keys.\nOther than that, I won't even tell you how to beat the game.||\nHave fun!\n\n",
			keygen3, strlen(keygen3), 35);

		system("pause");
		system("cls");

		second_game::main();
	}
	if (save_file.size() > 9) {
		for (size_t i = 0; i < save_file[9].size(); i++)
		{
			second_game::directions.push_back(save_file[9][i]);
		}
	}
	__sig(8);

	__xorsig(main, (const char*)second_game::directions.data(), second_game::directions.size(), 9);

	__sig(9);
	if (save_file.size() <= 9) {
		add_key((const char*)second_game::directions.data(), second_game::directions.size());
	}
	if (save_file.size() <= 10)
	{
		print_c("[Bouncer's arsenal]\n\nThe bouncer is back once again and you once again need to get past.\nThis time, he has a lot more tools up his sleeve...\n\n", (const char*)second_game::directions.data(), second_game::directions.size(), 35);

		troll::main();
		std::cout << '\n';
	}
	if (save_file.size() > 10) {
		troll::input_len = save_file[10].size();
		troll::input_cstr = new char[troll::input_len];
		for (size_t i = 0; i < troll::input_len; i++)
		{
			troll::input_cstr[i] = save_file[10][i];
		}
	}
	__sig(9);

	__xorsig(main, (const char*)troll::input_cstr, troll::input_len, 10);

	__sig(10);
	if (save_file.size() <= 10) {
		add_key(troll::input_cstr, troll::input_len);
	}
	if (save_file.size() <= 11)
	{
		print_c("[Virtual realm]\n\nMore custom bytecode.\nNo anti reverse engineering techniques.\nJust custom bytecode.\n\n", (const char*)troll::input_cstr, troll::input_len, 35);

		keygen4::main();
	}
	if (save_file.size() > 11) {
		keygen4::keylen = save_file[11].size();
		for (size_t i = 0; i < keygen4::keylen; i++)
		{
			keygen4::key[i] = save_file[11][i];
		}
	}
	__sig(10);

	__xorsig(main, (const char*)keygen4::key, keygen4::keylen, 11);

	__sig(11);
	if (save_file.size() <= 11) {
		add_key((const char*)keygen4::key, keygen4::keylen);
	}
	if (save_file.size() <= 12)
	{
		print_c("[Prison Break]\n\nAll the techniques broke out of prison and are all coming out to play one last time.\nAnd maybe more.\nStronger than ever.\n\n\nEven the bouncer is here!\n\n", (const char*)keygen4::key, keygen4::keylen, 35);
		
		end::main();
	}
	if (save_file.size() > 12) {
		end::keylen = save_file[12].size();
		end::key = new char[end::keylen];
		for (size_t i = 0; i < end::keylen; i++)
		{
			end::key[i] = save_file[12][i];
		}
	}
	__sig(11);

	__xorsig(main, (const char*)end::key, end::keylen, 12);

	__sig(12);
	if (save_file.size() <= 12) {
		add_key(end::key, end::keylen);
	}

	print_c("You made it...|\nThrough everything...|\nA while ago i promised that i would tell you the point of all this.||\n\n"
			"|||It was so that you could learn.|\nFight.|\nBecome stronger.|\nBecome better.|\n\n"
			"The only thing i have left is contact details, so that we may find each other again.|\n__xorkey3 (discord)\n\nGo out into the wild and have fun with your new skills.|||\nAu Revoir.||||||", 
		end::key, end::keylen, 40);

	__sig(12);


	delete[] key;
	Sleep(10000000);
	std::cout << ' ';
}















/*
credits

(games are in order of highest impact to lowest, everything else is not in order)

Bitburner
Entry Point
A Hat In Time
bloons td X
NGU Idle

Carlsagan42
Press Continue

And finally, crackmes.one!
*/
