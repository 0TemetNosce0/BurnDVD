
#include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "debuglog.h"


typedef void (*FDebugLog)(const wchar_t* log);

static FDebugLog gDebugLog = NULL;

void SetDebugLogFunction(FDebugLog DebugLog) {
	gDebugLog = DebugLog;
}

void debuglog(int line, const wchar_t* format, ...) {
    int nBuf;
    wchar_t szLog[1024];
    wchar_t szBuffer[1024];
    va_list args;

    va_start(args, format);
    nBuf = vswprintf_s(szLog, 1024, format, args) ;
    va_end(args);

    assert(nBuf >= 0);

    nBuf = swprintf_s(szBuffer, 1024, L"<%d>-%s", line, szLog);
    assert(nBuf >= 0);

    if (gDebugLog) {
    	gDebugLog(szBuffer);
    }
    else {
    	fwprintf(stderr, szBuffer);
    }
}
