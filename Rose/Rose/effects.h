#pragma once

// pretty sure this file ended up being not used although it's a cool effect

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

// difficulty can be between 1 and 100
std::string bitburner_text_scramble(const std::string& input, int difficulty) 
{
    std::string scrambled = input;

    LARGE_INTEGER seed;
    QueryPerformanceCounter(&seed);

    std::srand(static_cast<unsigned int>(seed.LowPart));

    for (char& c : scrambled) {
        int randomChance = std::rand() % 100;

        if (randomChance < difficulty)
        {
            if (randomChance % 3 == 0) {
                c = (std::rand() % 2 == 0) ? 'A' + std::rand() % 26 : 'a' + std::rand() % 26;
            }
            else if (randomChance % 3 == 1) {
                c = '0' + std::rand() % 10;
            }
            else {
                const std::string symbols = "!@#$%^&*()_+-=[]{}|;':,.<>?";
                c = symbols[std::rand() % symbols.size()];
            }
        }
    }

    return scrambled;
}



