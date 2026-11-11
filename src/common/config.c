#define WIN32_LEAN_AND_MEAN // xclude unnecessary Windows header declarations

#include <windows.h> // windows API
#include <wchar.h>
#include "common.h"

int config_get_value(
    const wchar_t *filePath, // const because they're input parameters, don't want to modify them.
    const wchar_t *key,
    wchar_t *output, // in this case, output is defined as a pointer. so, when we input, let's say, "result" in main.c, the function's output pointer now points to the first element of result. it doesnt need to re-allocate anything, as it was already allocated in main().
    size_t outputSize
) {
    if (!filePath || !key || !output || outputSize < 2) // check for filepath, key, or output being null, or the output size being less that 2, which is a null-terminated string. so, if the string needs to end in \0, then an array with 1 element is just an empty string
        return 0;

    if (outputSize > MAXDWORD) // MAXDWORD is defined by the windows api and it's just the 32bit unsigned integer limit. cant be bigger (it wont though lmao)
        return 0;

    DWORD len = GetPrivateProfileStringW( 
        L"Programs", // this is the name of section containing the key's name such as "[Programs]" on its own line in a .ini file.
        key, // the name of the key
        L"", // the default string. if it a string cannot be found on a key, it defaults to this.
        output, // this is the pointer to the buffer that recieves the retrieved string. because output points to the memory occupied by "result", windows will write directly into result. when config_get_values finishes, the output pointer parameter goes away, but the original result array in main() still exists.
        (DWORD)outputSize, // the size of the buffer pointed to by the output in characters. the expression itself is called a "typecast" and it explicitly converts the value from size_t to DWORD
        filePath // the path of the config file. we dont use *filePath or else we'd only get the single value at the pointer, which is the first value in the string.
    );

    if (len == 0 || len >= outputSize - 1) // if len is 0, then no characters were copied, and it also checks if the returned string was truncated because the output buffer was too small cause you need to take into account the null terminator
        return 0;

    return 1;
}