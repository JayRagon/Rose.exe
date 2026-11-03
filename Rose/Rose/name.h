#pragma once

#include <Windows.h>
#include <lmcons.h>
#include <iostream>
#include <string>

std::string get_users_name()
{
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;

    if (GetUserName(username, &username_len)) {
        return std::string(username);
    }
    else
    {
        return "Freelancer";
    }
}

