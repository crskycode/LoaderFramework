// main.cpp
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <pathcch.h>
#include <detours.h>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    WCHAR szDirPath[MAX_PATH];
    WCHAR szAppPath[MAX_PATH];

    GetModuleFileNameW(hInstance, szDirPath, ARRAYSIZE(szDirPath));
    if (GetLastError() != ERROR_SUCCESS)
        return 1;

    if (PathCchRemoveFileSpec(szDirPath, ARRAYSIZE(szDirPath)) != S_OK)
        return 1;

    if (PathCchAddBackslash(szDirPath, ARRAYSIZE(szDirPath)) != S_OK)
        return 1;

    if (PathCchCombine(szAppPath, ARRAYSIZE(szAppPath), szDirPath, L"akeiro.exe") != S_OK)
        return 1;

    STARTUPINFOW startupInfo = { sizeof(startupInfo) };
    PROCESS_INFORMATION processInfo = {};
    LPCSTR rgDlls[1] = { "akeiro.chs.dll" };

    if (DetourCreateProcessWithDllsW(szAppPath, NULL, NULL, NULL, FALSE, NULL, NULL, szDirPath, &startupInfo, &processInfo, ARRAYSIZE(rgDlls), rgDlls, NULL) != TRUE)
        return 1;

    return 0;
}
