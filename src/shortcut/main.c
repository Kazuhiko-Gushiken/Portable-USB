#define WIN32_LEAN_AND_MEAN // exclude unnecessary Windows header declarations

#include <windows.h> // windows API
#include <wchar.h>

#include "../common/common.h"

#define BUF_SIZE 32768 // hard coded buffer size, should hold the longest path, though with arguments, it can get longer. assume nothing massive will be used. need to implement dynamic buffer allocation in the future.
#define NAME_BUF_SIZE 256 // the max size of a file name, no need to reserve a lot of space.

// the L"" is a string. the L is saying its a wide character string. windows expects wchar_t strings, so we have to.

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR cmdLine, int showCmd) { // wWinMain defined here: https://learn.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point

    wchar_t self[BUF_SIZE]; // wchar_t is a data type for a string longer than 16 bits.

    wchar_t config[BUF_SIZE]; // the BUF_SIZE in square brackets is an element count. in this case, it reserves a "BUF_SIZE" amount of wide chracters for the "config" variable. in this line, "config" is the variable name or identifier used to access the block of memory in the program

    wchar_t root[BUF_SIZE];

    wchar_t fullTarget[BUF_SIZE];

    wchar_t name[NAME_BUF_SIZE];

    // finding the executable. DWORD is a windows API data type for a 32-bit unsigned integer
    DWORD len = GetModuleFileNameW(NULL, self, BUF_SIZE); // self is telling it to store the string in the "self" array. https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew

    if (!len || len >= BUF_SIZE) // check if windows failed or if the path is larger than the buff size. if so, end process with error code 1.
        return 1;

    // this extracts the executable name
    const wchar_t *base = wcsrchr(self, L'\\'); // here, we define a pointer to a constant wide characters called base (base can be changed to point somewhere else, but you cant modify the characters through base), and we use "wscrchr" which looks in the entire string "self" for the last occurance of the backslash, which is the last backslash in a path to define the path.
    base = base ? base + 1 : self; // here, we take that pointer, and we do a check. so, we take the base pointer, and we add one to the pointer, so the value infront of the backslash. but, if the base pointer is null, aka wcsrchr couldnt find a backslash, then it defaults to "self" which is all there is for the path.

    if (wcscpy_s(name, NAME_BUF_SIZE, base) != 0)
        return 1; // here, we're going to copy the executable name, base, and then feed it to the name array.

    wchar_t *ext = wcsrchr(name, L'.'); // this will take the copy of base, which is "name", and check for the last period in it, which should be the one right before the extension, and then define that into an extension pointer.
    if (ext)
        *ext = L'\0'; // if the ext's period is found, it's pointer value is replaced with a null terminator, to define it as the end. the extension is stil there in the array, but its practically ignored if we call upon the first value for the first string in the array.
    
    // now, we're going to find the root of the drive that the shortcut is in.
    if (!GetVolumePathNameW(self, root, BUF_SIZE)) // in this case, we're checking if it can even find the root. if not, it just returns 2. but since we're running it anyway to find out, the root is put into "root" if it can.
        return 2;

    if (wcscpy_s(config, BUF_SIZE, root) != 0)
        return 1;

    if (wcscat_s(config, BUF_SIZE, L"config\\launchers.conf")) // so, this will concatenate wide character strings. this case, we're defining a destination, which is config. this currently has our config in it (for example, "E:\"), but it ends in our null terminator. what wcscat_s does, it is find the null terminator, and starts writing at its position, adding the source, or the L"config\\launchers.ini" onto it, giving a full path to the config file. 
        return 3;

    wchar_t output[BUF_SIZE];

    if (!config_get_value(config, name, output, BUF_SIZE)) {// this will run the config_get_value function and then output the path. if it doesnt work, it opens a message box saying no matching program was found.
        MessageBoxW(NULL,
            L"No matching program found in launchers.conf. Check key names.",
            L"Portable Launcher", MB_ICONERROR);
        return 4;
        }

    // MessageBoxW(NULL, output, L"1. Raw INI Value", MB_OK);

    wchar_t *path = output; // define a wchar_t pointer of path for the output array.
    wchar_t *args = NULL; // define a wchar_t pointer that is null.

    if (output[0] == L'"') { //first, we'll check if the output array (the one that contains our entire path + args) starts with a quotation mark (index 0). if so, we're going to take the pointer path and add one. this starts the pointer path at index 1 of the output array.
        path = output + 1; // here, we take make the path itself start with the value right after the first quote, so it ignores it in the path pointer.
        
        wchar_t *end = wcschr(path, L'"'); // now, we take the path pointer (which is index 1 and skipping the first quotation mark) and then check for the first (next) quotation mark. this should be the "end" of the main path. this is defined as the pointer "end".

        if (!end) // if it cant find it, it exits.
            return 1;
        
        args = end + 1; // if it does find it, it will take the args pointer, and re-define it from null to the end + 1. which is the point where the space is.
        *end = L'\0'; // then, it replaces the end quotation mark with a null terminator.
    } else { // if it doesnt find a starting quotation mark, it runs this.
        wchar_t *space = wcschr(path, L' '); // now, we're going to look for the first space, which should be the end of the path. (because if the path itself has a space in it, it REQUIRES quotes around it.)

        if (space) { // if it finds it. it will make the args pointer the location of the space plus 1. then, it will replace the space itself with a null terminator to split the array into two strings.
            args = space + 1;
            *space = L'\0';
        }
    }

    while (args && *args == L' ') // if the args is not-null, and the args pointer points to a space, move one forward to the start of the args itself. since it's a while statement, if there are multiple spaces, it will move forward until it reaches a non-space.
        args++;

    // MessageBoxW(NULL, path, L"2. Parsed Path", MB_OK);

    if (!resolve_path(root, self, path, fullTarget, BUF_SIZE))
        return 1;

    // MessageBoxW(NULL, fullTarget, L"3. Resolved Path", MB_OK);

    if (!create_process(fullTarget, args))
        return 6;

    return 0;
}