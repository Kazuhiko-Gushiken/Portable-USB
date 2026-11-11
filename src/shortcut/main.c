#define WIN32_LEAN_AND_MEAN // prevent extras that the program doesnt need. this will make the executable much smaller
#include <windows.h> // windows API
#include <wchar.h>

#define BUF_SIZE 32768 // hard coded buffer size, should hold the longest path, though with arguments, it can get longer. assume nothing massive will be used. need to implement dynamic buffer allocation in the future.

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR cmdLine, int showCmd) { // wWinMain defined here: https://learn.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point
    wchar_t self[BUF_SIZE]; // wchar_t is a data type for a string longer than 16 bits.
    wchar_t root[BUF_SIZE]; // in this line, "root" is the variable name or identifier used to access the block of memory in the program
    wchat_t config[BUF_SIZE]; // the BUF_SIZE in square brackets is an element count. in this case, it reserves a "BUF_SIZE" amount of wide chracters for the "config" variable.
    wchat_t target[BUF_SIZE];
    wchar_t fullTarget[BUF_SIZE];
    wchar_t name[BUF_SIZE];

    // finding the executable. DWORD is a windows API data type for a 32-bit unsigned integer
    DWORD len = GetModuleFileNameW(NULL, self, BUF_SIZE); // self is telling it to store the string in the "self" array. https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew
    if (!len || len >= BUF_SIZE) // check if windows failed or if the path is larger than the buff size. if so, end process with error code 1.
        return 1;

    //  
}