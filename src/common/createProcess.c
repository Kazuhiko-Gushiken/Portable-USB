#define WIN32_LEAN_AND_MEAN // xclude unnecessary Windows header declarations

#include <windows.h> // windows API
#include <wchar.h>
#include "common.h"

#define BUF_SIZE 32768

int create_process(const wchar_t *executable, const wchar_t *args) {
    if (!executable || executable[0] == L'\0')
        return 0;

    wchar_t commandLine[BUF_SIZE];

    size_t exeLen = wcslen(executable); // looking at length of the executable
    size_t argsLen = args ? wcslen(args) : 0; // looking at length of the args if it exists.

    size_t required = exeLen + argsLen + 4; // combine them. add 4 for opening and closing quotes, the space between path and arguments, and then null terminator.

    if (required > BUF_SIZE) { // check if the required buffer size is larger than the hardcoded amount.
        return 0;
    }

    commandLine[0] = L'\0'; // we need to initialize the array with a null terminator.

    if (wcscat_s(commandLine, BUF_SIZE, L"\"") != 0) // add a quote
        return 0;

    if (wcscat_s(commandLine, BUF_SIZE, executable) != 0) // add the executable path
        return 0;

    if (wcscat_s(commandLine, BUF_SIZE, L"\"") != 0) // add another quote
        return 0;

    if (args && args[0] != L'\0') { // if there are args
        if (wcscat_s(commandLine, BUF_SIZE, L" ") != 0) //add a space
            return 0;

        if (wcscat_s(commandLine, BUF_SIZE, args) != 0) //add the args
            return 0;
    }

    wchar_t workingDir[BUF_SIZE];

    if (wcscpy_s(workingDir, BUF_SIZE, executable) != 0)
        return 0;

    wchar_t *lastSlash = wcsrchr(workingDir, L'\\');

    if (!lastSlash)
        return 0;
    
    *lastSlash = L'\0';

    STARTUPINFOW si = {0}; // this is a struct that windows uses. it contains a bunch of variables. setting it to {0} uses detaults.
    PROCESS_INFORMATION pi = {0}; // same as above, using defaults.
    si.cb = sizeof(si); // setting the cb variable inside of the struct.
    
    if (!CreateProcessW(
        executable, // full path of the executable
        commandLine, // writable command line string
        NULL, // default process security, not using
        NULL, // default thread security, not using
        FALSE, // dont inherit handles
        0, // no special creation flags
        NULL, // inherit parent's environment
        workingDir, // inherit parent's working directory
        &si, // memory address of the startup settings
        &pi // memory address for process information.
    )) {
        DWORD error = GetLastError();

        wchar_t message[32768];

        swprintf_s(
            message,
            32768,
            L"CreateProcessW failed.\n"
            L"Windows error: %lu\n\n"
            L"Executable:\n%ls",
            error,
            executable
        );

        MessageBoxW(
            NULL,
            message,
            L"Portable Launcher Error",
            MB_OK | MB_ICONERROR
        );
        return 0;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 1;
}