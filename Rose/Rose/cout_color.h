#pragma once

constexpr int COUT_GREEN = 10;
constexpr int COUT_RED = 4;
constexpr int COUT_PASTEL_RED = 12;
constexpr int COUT_BLUE = 1;
constexpr int COUT_PASTEL_BLUE = 9;
constexpr int COUT_MAGENTA = 13;
constexpr int COUT_BLACK = 0;
constexpr int COUT_WHITE = 7;

HANDLE HANDLE_STD_COUT = GetStdHandle(STD_OUTPUT_HANDLE);

void set_cout_color(int color)
{
	SetConsoleTextAttribute(HANDLE_STD_COUT, color);
}