#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#include <iostream>
#include <strsafe.h>

void ErrorExit(LPCTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}

/// <summary> Prints all Thread IDs in a specified process</summary>
/// <param name="dwProcessId">Process ID to filter by </param>
/// <returns>void</returns>
void EnumerateThreads(DWORD dwProcessId)
{
    BOOL bSuccess;
    HANDLE hSnapshot;
    THREADENTRY32 stThreadEntry;
    stThreadEntry.dwSize = sizeof(stThreadEntry);

    hSnapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPALL,    // dwFlags
        0                  // th32ProcessID
    );
    if (hSnapshot == INVALID_HANDLE_VALUE)
        ErrorExit(TEXT("CreateToolhelp32Snapshot"));

    bSuccess = Thread32First(hSnapshot, &stThreadEntry);
    if (bSuccess == FALSE)
        ErrorExit(TEXT("Thread32First"));

    std::cout << "Threads:" << std::endl;

    do 
    {
        if (stThreadEntry.th32OwnerProcessID == dwProcessId)
        {
            std::cout << "\tThread ID: " << stThreadEntry.th32ThreadID << std::endl;
        }
    } while (bSuccess = Thread32Next(hSnapshot, &stThreadEntry));

    CloseHandle(hSnapshot);
}

/// <summary> Prints all module in a specified process</summary>
/// <param name="dwProcessId">Process ID to filter by </param>
/// <returns>void</returns>
void EnumerateModules(DWORD dwProcessId)
{
    BOOL bSuccess;
    HANDLE hSnapshot;
    MODULEENTRY32 stModuleEntry;
    stModuleEntry.dwSize = sizeof(stModuleEntry);

    hSnapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE,
        dwProcessId
    );
    if (hSnapshot == INVALID_HANDLE_VALUE)
        ErrorExit(TEXT("CreateToolhelp32Snapshot"));

    bSuccess = Module32First(
        hSnapshot,
        &stModuleEntry
    );
    if (bSuccess == FALSE)
        ErrorExit(TEXT("Module32First"));

    std::cout << "Modules:"  << std::endl;

    do 
    {
        if (stModuleEntry.th32ProcessID == dwProcessId)
        {
            std::cout << "\tModule: " << stModuleEntry.szExePath << std::endl;
            _tprintf(TEXT("\n     Executable     = %s"), stModuleEntry.szExePath);
        }

    } while (bSuccess = Module32Next(hSnapshot, &stModuleEntry));

    CloseHandle(hSnapshot);
}

int main(void)
{
    HINSTANCE hPopCalc = LoadLibrary(TEXT("C:\\Users\\axelp\\source\\repos\\Target\\PopCalc_64.dll"));

    DWORD dwProcessId;
    dwProcessId = GetCurrentProcessId();

    std::cout << "Process ID: " << dwProcessId << std::endl;
    EnumerateThreads(dwProcessId);
    EnumerateModules(dwProcessId);

	return 0;
}