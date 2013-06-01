// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define _WIN32_WINNT 0x502

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Wspiapi.h>
#include <windows.h>

#ifdef _WIN64
#   define APP_X64_STRING   "x64"
#else
#   define APP_X64_STRING ""
#endif

