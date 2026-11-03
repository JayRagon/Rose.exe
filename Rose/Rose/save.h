#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>

#include <fstream>
#include <filesystem>
#include <string>

std::string read_save()
{
	std::filesystem::path current_folder = std::filesystem::current_path();
	std::string save_path = current_folder.string() + "\\save.txt";

	std::ifstream save(save_path);
	if (save.is_open()) {
		std::string current((std::istreambuf_iterator<char>(save)), std::istreambuf_iterator<char>());
		save.close();
		return current;
	}
	else {
		std::ifstream fallback_save("C:\\Users\\USER\\source\\repos\\Rose\\x64\\Release\\save.txt");
		if (fallback_save.is_open()) 
		{
			std::string fallback((std::istreambuf_iterator<char>(fallback_save)), std::istreambuf_iterator<char>());
			fallback_save.close();
			return fallback;
		}
		else
		{
			std::cout << "Could not find save.txt\n";
			return "";
		}
	}
}

// used in task 11 by the bouncer
// in retrospect this was really mean considering everything else that the player has already been through
// buuuuut i wanted this project to be as brutal as i could possibly make it... so i did this
// to be honest, re-doing the first 11 stages shouldn't take long since you've already done it
__declspec(noinline) void clear_save()
{
	std::filesystem::path current_folder = std::filesystem::current_path();
	std::string save_path = current_folder.string() + "\\save.txt";

	std::ofstream ofs(save_path, std::ofstream::out | std::ofstream::trunc);
	if (ofs.is_open())
	{
		ofs.close();
	}
	else {
		std::ofstream fallback("C:\\Users\\USER\\source\\repos\\Rose\\x64\\Release\\save.txt");
		fallback.close();
	}
}

std::vector<std::vector<char>> get_save_keys()
{
	std::vector<std::vector<char>> ret_buf;
	std::vector<std::string> hex;

	std::string save = read_save();

	uint32_t save_iterator = 0;

	for (size_t i = 0;; i++)
	{
		if (save_iterator >= save.length()) {
			break;
		}

		hex.push_back("");

		for (size_t u = 0;; u++)
		{
			if (save_iterator >= save.length()) {
				if (u == 0) { hex.pop_back(); }
				break;
			}

			if (save[save_iterator] != '\n') {
				hex[i] += save[save_iterator];
				save_iterator++;
			}
			else
			{
				if (u == 0) { hex.pop_back(); }

				save_iterator++;
				break;
			}
		}
	}
	

	for (size_t i = 0; i < hex.size(); i++)
	{
		std::vector<char> buf;
		buf.clear();
		for (size_t u = 0; u < hex[i].size(); u += 2)
		{
			char single_byte[3] = { hex[i][u], hex[i][u + 1], 0 };
			buf.push_back((char)std::stoi(single_byte, 0, 16));
		}
		ret_buf.push_back(buf);
	}

	return ret_buf;
}

void itoa_safe(unsigned char integer, char hex[3])
{
	char upper = integer >> 4;
	char lower = integer & 0b1111;

	if (upper < 10) {
		hex[0] = upper + 48;
	}
	else {
		hex[0] = upper + 55;
	}
	if (lower < 10) {
		hex[1] = lower + 48;
	}
	else {
		hex[1] = lower + 55;
	}
	hex[2] = 0;
}

// without null-terminator
void add_key(const char* key, uint32_t keysize)
{
	std::filesystem::path current_folder = std::filesystem::current_path();
	std::string save_path = current_folder.string() + "\\save.txt";

	if (std::filesystem::exists(save_path)) {
		std::ofstream save(save_path, std::fstream::app);

		for (size_t i = 0; i < keysize; i++)
		{
			char hex[3] = {};
			itoa_safe(key[i], hex);
			save << hex[0];
			save << hex[1];
		}

		save << '\n';

		save.close();
	}
	else {
		std::ofstream fallback_save("C:\\Users\\USER\\source\\repos\\Rose\\x64\\Release\\save.txt", std::fstream::app);
		if (fallback_save.is_open())
		{
			for (size_t i = 0; i < keysize; i++)
			{
				char hex[3] = {};
				itoa_safe(key[i], hex);
				fallback_save << hex[0];
				fallback_save << hex[1];
			}

			fallback_save << '\n';

			fallback_save.close();
		}
		else
		{
			std::cout << "Could not find save.txt\n";
		}
	}
}
