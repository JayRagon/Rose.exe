// alr so before you use this file
// nightshade only works without opti
// and cryptx works but stores strings in .text with lots of instructions

// more terrible naming!
// blueshade is a map on roblox bhop which i used to like
// same with greenshade
// redshade was made up
// nightshade is a character from roblox "entry point"

// i ended up mainly NOT using this .hpp file, since i just ended up using raw, normal strings then runtime decryption
// rosencryptor did the compile-time encryption work since i couldn't get the compile to cooperate with me
// so most strings are .text like normal and encrypted thanks to rosencryptor's post-processing!


#pragma once

#include <Windows.h>
#include <iostream>

#include <array>

#define blueshade(str, c_key) \
str_shade::enc<sizeof str - 1, sizeof c_key - 1>(str, c_key)

#define redshade(str, r_key, r_ksize) \
str_shade::dec<str.size()>(str, r_key, r_ksize)

// nightshade stores strings in .rdata (without opti...)
#define nightshade(name, str, c_key, r_key, r_ksize) \
static constexpr auto e_##name = blueshade(str, c_key); \
std::string name = redshade(e_##name, r_key, r_ksize)

namespace str_shade
{
    template <uint32_t size, uint32_t ksize>
    consteval std::array<char, size> enc(const char* raw_str, const char* key)
    {
        std::array<char, size> str = {};

        for (size_t i = 0; i < size; i++)
        {
            str[i] = raw_str[i] ^ key[i % ksize];
        }

        return str;
    }

    template <uint32_t size>
    std::string dec(std::array<char, size> encrypted, const char* key, uint32_t ksize)
    {
        std::string str = "";

        for (size_t i = 0; i < size; i++)
        {
            str += encrypted[i] ^ key[i % ksize];
        }

        return str;
    }

    void nuke(std::string& str)
    {
        for (size_t i = 0; i < str.length(); i++)
        {
            str[i] = (str[i] | (str[i] & (str[i] >> 1)));
        }
    }

    template <uint32_t c_str_size, uint32_t c_ksize>
    std::string bypass_define(const char* raw_str,
        const char* c_key,
        const char* r_key, uint32_t r_ksize)
    {
        // we are close, but not quite there yet
        static constexpr auto encrypted = str_shade::enc<c_str_size, c_ksize>("xyxyxyxy", "\x01");

        std::string decrypted = str_shade::dec<c_str_size>(encrypted, r_key, r_ksize);
        return decrypted;
    }
}


// do not nest bleeds for obfuscation
// this stores strings in .text with heaps of movdqa instructions or similar
#define cryptz(x, k) \
strcryptx::cryptx<sizeof(x), sizeof(k) - 1>(x, k)

// simply uses the encryption at compile time and decryption at runtime for obfuscation
#define cryptx_obf(x, k) \
cryptz(x, k).dec(k)


#pragma optimize("", off)
namespace strcryptx
{
    template <uint32_t data_size, uint32_t key_size>
    class cryptx
    {
    public:
        consteval cryptx(const char* str, const char* k)
        {
            for (size_t i = 0; i < data_size; i++)
            {
                data[i] = str[i] ^ k[i % key_size];
            }

            key_size_local = key_size;
            data_size_local = data_size;
        }

        __declspec(noinline) void nuke()
        {
            for (size_t i = 0; i < data_size; i++)
            {
                data[i] = 0x00;
            }
        }

        __declspec(noinline) char* dec(const char* k)
        {
            for (size_t i = 0; i < data_size_local; i++)
            {
                data[i] ^= k[i % key_size_local];
            }

            if (data_size_local > 0) {
                data[data_size_local - 1] = 0;
            }

            return data;
        }

        uint32_t data_size_local;
        uint32_t key_size_local;
        char data[data_size] = {};
    };
};
#pragma optimize("", on)


