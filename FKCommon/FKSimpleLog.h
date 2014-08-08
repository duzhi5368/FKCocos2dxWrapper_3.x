//*************************************************************************
//	创建日期:	2014-8-7   10:09
//	文件名称:	FKSimpleLog.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include <stdio.h>

#if defined(WIN32) || defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif

#pragma warning( disable:4996 )
//-------------------------------------------------------------------------
static void _FKSimpleLog(const char *format, va_list args)
{
	char buf[16*1024];

	vsnprintf(buf, (16*1024)-3, format, args);
	strcat(buf, "\n");

#if defined(WIN32)
	WCHAR wszBuf[16*1024] = {0};
	MultiByteToWideChar(CP_UTF8, 0, buf, -1, wszBuf, sizeof(wszBuf));
	OutputDebugStringW(wszBuf);
	WideCharToMultiByte(CP_ACP, 0, wszBuf, -1, buf, sizeof(buf), nullptr, FALSE);
	printf("%s", buf);
	fflush(stdout);
#else
	// Linux, Mac, iOS, etc
	fprintf(stdout, "Log: %s", buf);
	fflush(stdout);
#endif
}
//-------------------------------------------------------------------------
inline void FKSimpleLog(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	_FKSimpleLog(format, args);
	va_end(args);
}
//-------------------------------------------------------------------------