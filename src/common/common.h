#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>
#include <wchar.h>


// the "int" in front doesn't define what the function processes, but what it outputs. it'll output a status code.
// the asterisk means that it's a pointer to the string containing the name.
int config_get_value(const wchar_t *filePath, const wchar_t *key, wchar_t *output, size_t outputSize);
// example:
// config_get_value(L"config.ini", L"OBS-4K", result, 256);

int resolve_path(const wchar_t *root, const wchar_t *self, const wchar_t *programPath, wchar_t *output, size_t outputSize); // this will take the root, the program path, and then resolve the path accordingly, and then put it into output.

int create_process(const wchar_t *executable, const wchar_t *args); // take in the executable path and the arguments.

#endif