#pragma once

// this header file is diabolical
// i'm sure theres a better way to do this... but i didn't care at the time
// again, i just wanted something that *worked* and was simple
// next time i need to do this for some reason i WILL be doing something better than this
// oh right! i did try to inject bytes into the .exe via rosencryptor without any bytes in the first place
// but this would be pretty hard dealing with all the relative addressing. 
// i tried to use zydis but i realised the scope that i'm using it for doesn't need it
// and i can just use nops instead...
// still, this needs to have a better way of doing it

#include <iostream>
#include <Windows.h>

#include <intrin.h>

#include "sigs.hpp"

#pragma region nops
#define _p __nop();

#define __nop2() \
_p _p 
#define __nop3() \
_p _p _p
#define __nop4() \
_p _p _p _p
#define __nop5() \
_p _p _p _p _p
#define __nop6() \
_p _p _p _p _p _p
#define __nop7() \
_p _p _p _p _p _p _p 
#define __nop8() \
_p _p _p _p _p _p _p _p 
#define __nop9() \
_p _p _p _p _p _p _p _p _p


#define __nop32() \
_p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p 

#define __nop64() \
_p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p 

#define __nop128() \
_p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p 

#define __nop256() \
_p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p 

#define __nop512() \
_p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p

#define __nop1024() \
_p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p _p 

#pragma endregion

// uses the same signature system as __sig
// this field of NOPs will be patched by rosencryptor when it runs with inline assembly
// this is a workaround since microsoft decided they didn't care about x64 inline assembly and didn't add it...
// take that, microsoft!
#define inline_asm(id, size) \
__sig((id + 0x1000)); __inline_asm(size);

// 2 bytes to 40 bytes, else 2^x up to 2048
// possibly a way to make it 2 to 2048 continuous
// (by adding bytes into the exe from rosencrypter instead of in here)
__forceinline void __inline_asm(uint32_t size)
{
	switch (size) {
	case 1:
		__nop();
	case 2:
		__nop2();
		break;
	case 3:
		__nop3();
		break;
	case 4:
		__nop4();
		break;
	case 5:
		__nop5();
		break;
	case 6:
		__nop6();
		break;
	case 7:
		__nop7();
		break;
	case 8:
		__nop8();
		break;
	case 9:
		__nop9();
		break;
	case 10:
		__nop();
		__nop9();
		break;
	case 11:
		__nop2();
		__nop9();
		break;
	case 12:
		__nop3();
		__nop9();
		break;
	case 13:
		__nop4();
		__nop9();
		break;
	case 14:
		__nop5();
		__nop9();
		break;
	case 15:
		__nop6();
		__nop9();
		break;
	case 16:
		__nop7();
		__nop9();
		break;
	case 17:
		__nop8();
		__nop9();
		break;
	case 18:
		__nop9();
		__nop9();
		break;
	case 19:
		__nop9();
		__nop9();
		__nop();
		break;
	case 20:
		__nop9();
		__nop9();
		__nop2();
		break;
	case 21:
		__nop9();
		__nop9();
		__nop3();
		break;
	case 22:
		__nop9();
		__nop9();
		__nop4();
		break;
	case 23:
		__nop9();
		__nop9();
		__nop5();
		break;
	case 24:
		__nop9();
		__nop9();
		__nop6();
		break;
	case 25:
		__nop9();
		__nop9();
		__nop7();
		break;
	case 26:
		__nop9();
		__nop9();
		__nop8();
		break;
	case 27:
		__nop9();
		__nop9();
		__nop9();
		break;
	case 28:
		__nop9();
		__nop9();
		__nop9();
		__nop();
		break;
	case 29:
		__nop9();
		__nop9();
		__nop9();
		__nop2();
		break;
	case 30:
		__nop9();
		__nop9();
		__nop9();
		__nop3();
		break;
	case 31:
		__nop9();
		__nop9();
		__nop9();
		__nop4();
		break;
	case 32:
		__nop32();
		break;
	case 33:
		__nop32();
		__nop();
		break;
	case 34:
		__nop32();
		__nop2();
		break;
	case 35:
		__nop32();
		__nop3();
		break;
	case 36:
		__nop32();
		__nop4();
		break;
	case 37:
		__nop32();
		__nop5();
		break;
	case 38:
		__nop32();
		__nop6();
		break;
	case 39:
		__nop32();
		__nop7();
		break;
	case 40:
		__nop32();
		__nop8();
		break;
	case 64:
		__nop64();
		break;
	case 128:
		__nop128();
		break;
	case 256:
		__nop256();
		break;
	case 512:
		__nop512();
		break;
	case 1024:
		__nop1024();
		break;
	case 2048:
		__nop1024();
		__nop1024();
		break;
	case 0x1000:
		__nop1024();
		__nop1024();
		__nop1024();
		__nop1024();
		break;
	default:
		std::cout << "bad int inline assembly\n";
	}
}


