#pragma once
#include <windows.h>
#include <stdio.h>

void encode_to_base64(char *dbuf, char *src, int len);
char convert_to_base64_char(char uc);