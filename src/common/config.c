#define WIN32_LEAN_AND_MEAN // xclude unnecessary Windows header declarations

#include <windows.h> // windows API
#include <stdio.h>
#include <stddef.h>
#include <wchar.h>

#include "common.h"

#define LINE_SIZE 4096

int config_get_value(
    const wchar_t *filePath, // const because they're input parameters, don't want to modify them.
    const wchar_t *key,
    wchar_t *output, // in this case, output is defined as a pointer. so, when we input, let's say, "result" in main.c, the function's output pointer now points to the first element of result. it doesnt need to re-allocate anything, as it was already allocated in main().
    size_t outputSize
) {
    FILE *file = NULL;

    if (!filePath || !key || !output || outputSize < 2) // check for filepath, key, or output being null, or the output size being less that 2, which is a null-terminated string. so, if the string needs to end in \0, then an array with 1 element is just an empty string
        return 0;

    output[0] = L'\0'; // setup the output array with the null terminator

    if (_wfopen_s(&file, filePath, L"r, ccs=UTF-8") != 0) // we're going to open the file at the file path. the &file lets us change the position of the pointer to the location of the file itself. the L"r" is for "read".
        return 0;
    
    wchar_t line[LINE_SIZE]; // define a line size so we can read it.

    while (fgetws(line, LINE_SIZE, file)) { // go through each line in the config file
        size_t len = wcslen(line); // get the length of the line.

        if (len == LINE_SIZE - 1 && line[len - 1] != L'\n') { // check if the line is the size of the buffer including the null terminator so, we need to check the next character to see if its there. then that the line's array is the amount minus 1 is NOT a \n. if so, the line is cut off.
            wint_t ch = fgetwc(file); // we're going to get the next wide character in the file.

            if (ch != L'\n' && ch != WEOF) { // now we check that character. if its not a \n, and its not an the end of a wide-character stream, then we move on.
                while((ch = fgetwc(file)) != WEOF && ch != L'\n') { // so, we get get one character at a time form the file, then we check if the reading failed or if its at the end of the file. if not, and that the character is not an \n, we keep doing this until we reach the end which will then trigger the end line. we want to go to the end or else the next iteration could treat leftover character as a new config entry.

                }
                fclose(file); // close the file
                MessageBoxW(NULL, // give error.
                    L"Line size is too long.",
                    L"Portable Launcher", MB_ICONERROR);
                return 0;
            }
        }

        line[wcscspn(line, L"\r\n")] = L'\0'; // this will search through the line and then find the first occurance of any character in that second string. in this case, it's carriage return and newline. then, it puts a null terminator at the first occurance and ends the string right there.

        wchar_t *equals = wcschr(line, L'='); // look for the first character in the line that is an equal sign

        if (!equals) // if there is none, skip the line. this handles invalid lines, blank lines, and comment lines.
            continue;
        
        *equals = L'\0'; // make the equal sign a null terminator to

        wchar_t *lineKey = line; // set the key to the line, or the first bit before the null terminator
        wchar_t *lineValue = equals + 1; // and then set the line value to the pointer right after the equal sign position.

        size_t keyLen = wcslen(lineKey); // this will define the length of the lineKey so we can jump around it via the index.

        while (keyLen > 0 && (lineKey[keyLen - 1] == L' ' || lineKey[keyLen - 1] == L'\t')) { // here, we're checking through the line. so long as the line is larger than 0, we're gonna keep going. we check the last character of the key first. if it's a space or the \t, we remove it. and replace with a null terminator. once we reach the first real character (not a whitespace) we stop.
            lineKey[--keyLen] = L'\0'; // replacing the position in the string with a null terminator --keyLen subtracts 1 before using it as an array index.
        }

        while (*lineKey == L' ' || *lineKey == L'\t') { // this takes the key, and checks the beginning of it. if it starts with a whitespace, it moves the pointer forward one.
            lineKey++;
        }

        while (*lineValue == L' ' || *lineValue == L'\t') { // so this checks for the pointer position for the value. if its a space, it just moves the pointer for lineValue forward to have the string pointer start forward.
            lineValue ++;
        }

        size_t valueLen = wcslen(lineValue);

        while (valueLen > 0 && (lineValue[valueLen - 1] == L' ' || lineValue[valueLen - 1] == L'\t')) { // this does the same thing as above with our key, but with the value, removing whitespaces from the end.
            lineValue[--valueLen] = L'\0';
        }

        if (wcscmp(lineKey, key) == 0) { // this checks if the key in the line matches the key the function is fed. if so...
            if (wcscpy_s(output, outputSize, lineValue) != 0) { // it then copies the line's value (after the equal sign) into the output. if it doesnt work, close file and error.
                fclose(file);
                return 0;
            }

            fclose(file); // if it does work, close file and woohoo
            return 1;
        }
    }

    int readError = ferror(file);

    fclose(file);

    if (readError) {
        MessageBoxW(NULL,
        L"Error reading configuration file.",
        L"Portable Launcher",
        MB_ICONERROR);
    }

    return 0;
}