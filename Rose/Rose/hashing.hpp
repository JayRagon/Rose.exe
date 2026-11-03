#pragma once

// a header file for hashingggg
// this was usually used for the keys
// compile time hashing was necessary so that people wouldn't just look at the code and find the key and inject it

#include <iostream>
#include <Windows.h>

// the CH-A was a gun in the roblox game "entry point"
// for this project i just used whatever name i thought was cool...
// but i guess the CH could mean Compile-time Hash
// then the -r at the end means runtime?
// yeah not a great naming convention...

// CH-A compile-time
#define ch_a(x) charred::hash<sizeof(x) - 1>(x)
// CH-R runtime
#define ch_r(x, s) runtime_charring::hash(x, s)

namespace charred
{
    template <uint32_t data_size>
    class hash
    {
    public:
        consteval hash(const char* str)
        {
            for (size_t i = 0; i < data_size; i++)
            {
                data[i] = (unsigned char)str[i];
            }

            // interpreter farts when you do too many rounds
            for (size_t x = 0; x < 100 / (data_size); x++)
            {
                for (size_t i = 0; i < data_size; i++)
                {
                    data[i] += data[data[0] % data_size];
                    rot();
                    data[0] += ((unsigned char)(i & 0xff) ^ data[i]);
                    rot();
                }
            }

            data[0] = 0;
        }

        consteval void rot()
        {
            unsigned char rot_buf = 0;
            unsigned char carry = 0;

            for (size_t i = 0; i < data_size; i++)
            {
                rot_buf = data[i] & 1;

                data[i] = data[i] >> 1;

                // force bit
                data[i] &= ~(carry << 7);
                data[i] |= (carry << 7);

                carry = rot_buf;
            }
            data[0] &= ~(carry << 7);
            data[0] |= (carry << 7);
        }

        uint32_t data_size_buffer = data_size;
        unsigned char data[data_size] = {};
    };
};

namespace runtime_charring
{
    void rot(unsigned char* data, uint32_t data_size)
    {
        unsigned char rot_buf = 0;
        unsigned char carry = 0;

        for (size_t i = 0; i < data_size; i++)
        {
            rot_buf = data[i] & 1;

            data[i] = data[i] >> 1;

            // force bit
            data[i] &= ~(carry << 7);
            data[i] |= (carry << 7);

            carry = rot_buf;
        }
        data[0] &= ~(carry << 7);
        data[0] |= (carry << 7);
    }

    void hash(unsigned char* data, uint32_t data_size)
    {
        for (size_t x = 0; x < 100 / (data_size); x++)
        {
            for (size_t i = 0; i < data_size; i++)
            {
                data[i] += data[data[0] % data_size];
                rot(data, data_size);
                data[0] += ((unsigned char)(i & 0xff) ^ data[i]);
                rot(data, data_size);
            }
        }
        data[0] = 0;
    }
};




// CMP hashes
bool ch_cmp(BYTE* x, BYTE* y, uint32_t size) {
    for (size_t i = 0; i < size; i++)
    {
        if (x[i] != y[i]) {
            return false;
        }
    }
    return true;
}
