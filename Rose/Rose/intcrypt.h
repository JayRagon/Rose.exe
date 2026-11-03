#pragma once 

#include <iostream>
#include <Windows.h>

// compile time integer encryption for obfuscation
template <typename T>
consteval T iobf_c(T i_input)
{
	return (i_input ^ 0xEE) + 0xE43E;
}


// runtime integer encryption for obfuscation
template <typename T>
T iobf_r(T i_input)
{
	return (i_input ^ 0xEE) + 0xE43E;
}

// decrypt the obfuscated integers
template <typename T>
T dcri(T i_input)
{
	return (i_input - 0xE43E) ^ 0xEE;
}