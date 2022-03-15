#include "base64.h"
#include "pch.h"

char convert_to_base64_char(char uc)
{
	if (uc < 26) {
		return 'A' + uc;
	}
	if (uc < 52 && uc >= 26) {
		return 'a' + uc - 26;
	}
	if (uc < 62 && uc >= 52) {
		return '0' + (uc - 52);
	}
	if (uc == 62) {
		return '+';
	}
	return '/';
}

void encode_to_base64(char *dbuf, char *src, int len)
{
	char t = '\0', t2 = '\0', t3 = '\0';
	int i = 0;
	for (i = 0; i < len / 3; i++) {
		t = src[i * 3 + 0];
		dbuf[i * 4 + 0] = convert_to_base64_char(((t >> 2) & 0x3f));///1
		memset(&t, 0, sizeof(char));

		t2 = src[i * 3 + 0];
		t3 = src[i * 3 + 1];
		dbuf[i * 4 + 1] = convert_to_base64_char(((((t2 << 6 & 0xc0) >> 2) & 0x30) | (t3 >> 4 & 0x0f)));///2
		memset(&t2, 0, sizeof(char));
		memset(&t3, 0, sizeof(char));

		t2 = src[i * 3 + 1];
		t3 = src[i * 3 + 2];
		dbuf[i * 4 + 2] = convert_to_base64_char(((((t2 << 4) & 0xf0) >> 2) & 0x3c) | ((t3 >> 6) & 0x03));///3
		memset(&t2, 0, sizeof(char));
		memset(&t3, 0, sizeof(char));

		t = src[i * 3 + 2];
		dbuf[i * 4 + 3] = convert_to_base64_char((((t << 2) & 0xfc) >> 2) & 0x3f);///4
		memset(&t, 0, sizeof(char));
	}
	if (len % 3 == 1) {
		t = src[i * 3 + 0];
		dbuf[i * 4 + 0] = convert_to_base64_char((t >> 2) & 0x3f);
		memset(&t, 0, sizeof(char));

		t = src[i * 3 + 0];
		dbuf[i * 4 + 1] = convert_to_base64_char((((t << 6) & 0xc0) >> 2) & 0x30);

		dbuf[i * 4 + 2] = '=';
		dbuf[i * 4 + 3] = '=';
		memset(&t, 0, sizeof(char));
		return;
	}
	if (len % 3 == 2) {
		t2 = src[i * 3 + 0];
		dbuf[i * 4 + 0] = convert_to_base64_char((t2 >> 2) & 0x3f);
		memset(&t2, 0, sizeof(char));

		t2 = src[i * 3 + 0];
		t3 = src[i * 3 + 1];
		dbuf[i * 4 + 1] = convert_to_base64_char(((((t2 << 6 & 0xc0) >> 2) & 0x30) | (t3 >> 4 & 0x0f)));
		memset(&t2, 0, sizeof(char));
		memset(&t3, 0, sizeof(char));

		t3 = src[i * 3 + 1];
		dbuf[i * 4 + 2] = convert_to_base64_char((t3 << 4 & 0xf0) >> 2 & 0x3c);
		memset(&t3, 0, sizeof(char));

		dbuf[i * 4 + 3] = '=';
		return;
	}
}