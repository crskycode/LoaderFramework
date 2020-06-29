// util.h
#pragma once


//=============================================================================
// Windows SDK Headers
//=============================================================================


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <intrin.h>
#include <pathcch.h>
#include <strsafe.h>
#include <atlstr.h>
#include <atlfile.h>


//=============================================================================
// Detour Header
//=============================================================================


#include <detours.h>


//=============================================================================
// C++ Runtime Headers
//=============================================================================


#include <algorithm>
using std::remove_if;
using std::remove_copy_if;


#include <string>
using std::string;
using std::wstring;


#include <memory>
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;


#include <list>
using std::list;


#include <vector>
using std::vector;


#include <map>
using std::map;


#include <unordered_map>
using std::unordered_map;


#include <set>
using std::set;


//=============================================================================
// Logger
//=============================================================================


void LogInit(LPCWSTR lpFileName);
void LogWrite(LPCSTR lpMessage, ...);
void LogWrite(LPCWSTR lpMessage, ...);


//=============================================================================
// Pattern Search Helper
//=============================================================================


PVOID GetModuleBase(HMODULE hModule);
DWORD GetModuleSize(HMODULE hModule);
PVOID SearchPattern(PVOID lpStartAddr, DWORD dwSearchLen, PCSTR lpPattren, DWORD dwPatternLen);


//=============================================================================
// Patch Helper
//=============================================================================


void PatchRead(LPVOID lpAddr, LPVOID lpBuf, DWORD nSize);
void PatchWrite(LPVOID lpAddr, LPVOID lpBuf, DWORD nSize);


template<class T>
void PatchRead(LPVOID lpAddr, T& lpBuf)
{
    PatchRead(lpAddr, &lpBuf, sizeof(T));
}


template<class T>
void PatchWrite(LPVOID lpAddr, T&& lpBuf)
{
    PatchWrite(lpAddr, &lpBuf, sizeof(T));
}


//=============================================================================
// Hook Helper
//=============================================================================


template<class T>
void InlineHook(T& OriginalFunction, T DetourFunction)
{
    DetourUpdateThread(GetCurrentThread());
    DetourTransactionBegin();
    DetourAttach(&(PVOID&)OriginalFunction, (PVOID&)DetourFunction);
    DetourTransactionCommit();
}


template<class T>
void UnInlineHook(T& OriginalFunction, T DetourFunction)
{
    DetourUpdateThread(GetCurrentThread());
    DetourTransactionBegin();
    DetourDetach(&(PVOID&)OriginalFunction, (PVOID&)DetourFunction);
    DetourTransactionCommit();
}


//=============================================================================
// Encoding
//=============================================================================


CStringW AnsiToUcs2(int cp, const CStringA& str);
CStringA Ucs2ToAnsi(int cp, const CStringW& str);
CStringW Utf8ToUcs2(const CStringA& str);
CStringA Ucs2ToUtf8(const CStringW& str);
CStringW ShiftJisToUcs2(const CStringA& str);
CStringA Ucs2ToShiftJis(const CStringW& str);
CStringW GbkToUcs2(const CStringA& str);
CStringA Ucs2ToGbk(const CStringW& str);
