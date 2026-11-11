#define WIN32_LEAN_AND_MEAN // exclude unnecessary Windows header declarations

#include <windows.h> // windows API
#include <wchar.h>
#include "common.h"
#include <pathcch.h>

#define BUF_SIZE 32768

int resolve_path(const wchar_t *root,
    const wchar_t *self,
    const wchar_t *programPath,
    wchar_t *output,
    size_t outputSize)
    {
        if (!root || !self || !programPath || !output) { // check that all of out inputs (and outputs) are non-null.
            return 0;
        }

        if (outputSize == 0) { // if the output size is 0, it's an invalid output buffer size.
            return 0;
        }

        if (programPath[0] == L'\0') { // if the array contains an empty string, the first index is just a null terminator. if so, no need to continue.
            return 0;
        }

        if (programPath[0] == L'\\' && programPath[1] == L'\\') { // if the path starts with \\ which is used for UNC network paths.
            return 0;
        }

        if (((programPath[0] >= L'A' && programPath[0] <= L'Z') || (programPath[0] >= L'a' && programPath[0] <= L'z')) && programPath[1] == L':' && programPath[2] != L'\\') { // if the path starts with C: and not C:\ 
            return 0;
        }

        if (wcsncmp(programPath, L".\\", 2) == 0 || wcsncmp(programPath, L"..\\", 3) == 0) { // wcsncmp compares multiple wide characters from two pointers, or in this case, a pointer and a string. here, it compares programPath and either ".\" or "..\" and if either match, right away, it outputs 0. then, we can normalize. if not, then we dont have relative paths relative to the shortcut's directory.
            wchar_t base[BUF_SIZE]; // simply define the base array

            if (wcscpy_s(base, BUF_SIZE, self) != 0) // copy the self array into the base pointer so we can mess with that.
                return 0;
            
            if (FAILED(PathCchRemoveFileSpec(base, BUF_SIZE))) // we're going to take the pointer at base, and then we're going to remove the file name (aka everything following the last backslash and the last backslash)
                return 0;
            
            if (FAILED(PathCchCombineEx( // this operation and the one above it use HRESULT, if it suceeeds, it outputs >= 0, but if it fails it is < 0. the FAILED or SUCEEDED just determines that easier. it's a windows api thing. pathcchcombineex takes a pointer to output to, the size of the buffer for the output, the pointer to the first path string, which is base that we fixed and got all nice. and then the programPath which is the leftover (with the relative things, which this function will take care of). PATHCCH_NONE just means it does nothing extra.
                output, outputSize, base, programPath, PATHCCH_NONE
            )))
                return 0;
        } else {
            const wchar_t *relative = programPath;

            if (((programPath[0] >= L'A' && programPath[0] <= L'Z') || (programPath[0] >= L'a' && programPath[0] <= L'z')) && programPath[1] == L':' && programPath[2] == L'\\') { // big one here! so, the first ones with the index 0 and the letters is just checking if the value in index 0 of the programPath is a letter. letters in an array are defined by a value, so we can treat them as numbers and do a numerical check. then we check programPath[1] to check for the colon, and then the third index spot for the backslash.
                relative = programPath + 3; // if so, we take the original programPath and add 3 to a pointer so it starts with the path right after the root.
            } else if (programPath[0] == L'\\') { // this checks if the first index spot is a backslash.
                relative = programPath + 1;
            }
            
            // now, all three possibilities are defined in relative and start with the true path, no root.

            if (FAILED(PathCchCombineEx( // this will now take the relative path we have, and plop it onto the root we have (the one bsed on the shortcut). then, it puts it in the output.
                output, outputSize, root, relative, PATHCCH_NONE
            ))) {
                return 0;
            }
        }

        return 1;
}