// util.cpp

#include "util.h"


//=============================================================================
// Logger
//=============================================================================


static CAtlFile gLogFile;


void LogInit(LPCWSTR lpFileName)
{
    gLogFile.Create(lpFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_ALWAYS);
}


void LogWrite(LPCSTR lpMessage, ...)
{
    CStringA str;
    va_list args;

    va_start(args, lpMessage);
    str.FormatV(lpMessage, args);
    va_end(args);

    CStringW ucs = AnsiToUcs2(CP_ACP, str);
    CStringA utf = Ucs2ToUtf8(ucs);

    gLogFile.Write(utf.GetString(), utf.GetLength());
}


void LogWrite(LPCWSTR lpMessage, ...)
{
    CStringW str;
    va_list args;

    va_start(args, lpMessage);
    str.FormatV(lpMessage, args);
    va_end(args);

    CStringA utf = Ucs2ToUtf8(str);
    gLogFile.Write(utf.GetString(), utf.GetLength());
}


//=============================================================================
// Pattern Search Helper
//=============================================================================


PVOID GetModuleBase(HMODULE hModule)
{
    MEMORY_BASIC_INFORMATION mem;

    if (!VirtualQuery(hModule, &mem, sizeof(mem)))
        return NULL;

    return mem.AllocationBase;
}


DWORD GetModuleSize(HMODULE hModule)
{
    return ((IMAGE_NT_HEADERS*)((DWORD_PTR)hModule + ((IMAGE_DOS_HEADER*)hModule)->e_lfanew))->OptionalHeader.SizeOfImage;
}


PVOID SearchPattern(PVOID lpStartAddr, DWORD dwSearchLen, PCSTR lpPattren, DWORD dwPatternLen)
{
    DWORD_PTR dwStartAddr = (DWORD_PTR)lpStartAddr;
    DWORD_PTR dwEndAddr = dwStartAddr + dwSearchLen - dwPatternLen;

    while (dwStartAddr < dwEndAddr)
    {
        BOOL bFound = TRUE;

        for (DWORD i = 0; i < dwPatternLen; i++)
        {
            CHAR code = *(PCHAR)(dwStartAddr + i);

            if (lpPattren[i] != 0x2A && lpPattren[i] != code)
            {
                bFound = FALSE;
                break;
            }
        }

        if (bFound)
            return (PVOID)dwStartAddr;

        dwStartAddr++;
    }

    return NULL;
}


//=============================================================================
// Patch Helper
//=============================================================================


void PatchRead(LPVOID lpAddr, LPVOID lpBuf, DWORD nSize)
{
    DWORD dwProtect;
    if (VirtualProtect(lpAddr, nSize, PAGE_EXECUTE_READWRITE, &dwProtect))
    {
        CopyMemory(lpBuf, lpAddr, nSize);
        VirtualProtect(lpAddr, nSize, dwProtect, &dwProtect);
    }
}


void PatchWrite(LPVOID lpAddr, LPVOID lpBuf, DWORD nSize)
{
    DWORD dwProtect;
    if (VirtualProtect(lpAddr, nSize, PAGE_EXECUTE_READWRITE, &dwProtect))
    {
        CopyMemory(lpAddr, lpBuf, nSize);
        VirtualProtect(lpAddr, nSize, dwProtect, &dwProtect);
    }
}


//=============================================================================
// Encoding
//=============================================================================


CStringW AnsiToUcs2(int cp, const CStringA& str)
{
    if (str.GetLength() == 0)
        return CStringW();
    int nLen = MultiByteToWideChar(cp, 0, str.GetString(), str.GetLength(), NULL, 0);
    if (nLen == 0)
        return CStringW();
    CStringW ret(L'\0', nLen);
    if (MultiByteToWideChar(cp, 0, str.GetString(), str.GetLength(), ret.GetBuffer(), ret.GetAllocLength()) == 0)
        return CStringW();
    return ret;
}


CStringA Ucs2ToAnsi(int cp, const CStringW& str)
{
    if (str.GetLength() == 0)
        return CStringA();
    int nLen = WideCharToMultiByte(cp, 0, str.GetString(), str.GetLength(), NULL, 0, NULL, NULL);
    if (nLen == 0)
        return CStringA();
    CStringA ret('\0', nLen);
    if (WideCharToMultiByte(cp, 0, str.GetString(), str.GetLength(), ret.GetBuffer(), ret.GetAllocLength(), NULL, NULL) == 0)
        return CStringA();
    return ret;
}


CStringW Utf8ToUcs2(const CStringA& str)
{
    return AnsiToUcs2(CP_UTF8, str);
}


CStringA Ucs2ToUtf8(const CStringW& str)
{
    return Ucs2ToAnsi(CP_UTF8, str);
}


CStringW ShiftJisToUcs2(const CStringA& str)
{
    return AnsiToUcs2(932, str);
}


CStringA Ucs2ToShiftJis(const CStringW& str)
{
    return Ucs2ToAnsi(932, str);
}


CStringW GbkToUcs2(const CStringA& str)
{
    return AnsiToUcs2(936, str);
}


CStringA Ucs2ToGbk(const CStringW& str)
{
    return Ucs2ToAnsi(936, str);
}
