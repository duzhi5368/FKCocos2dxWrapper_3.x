//*************************************************************************
//	创建日期:	2014-8-4   15:23
//	文件名称:	FKCommonDef.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include <assert.h>
#include "FKSimpleLog.h"
//-------------------------------------------------------------------------
#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif
//-------------------------------------------------------------------------
#ifndef NULL
	#ifdef __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
#endif
//-------------------------------------------------------------------------
// 常用类型定义
typedef unsigned char			uint8;
typedef unsigned short			uint16;
typedef unsigned int			uint32;

typedef float					float32;
typedef double					float64;

typedef char					int8;
typedef short					int16;
typedef int						int32;
//-------------------------------------------------------------------------
#if defined(_MSC_VER) && (_MSC_VER >= 1200 )
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <hash_map>

	typedef __int64             longlong;
	typedef __int64             int64;
	typedef unsigned __int64    uint64;
#else
	#include <ext/hash_map>

	#define stdext __gnu_cxx
	#define __LINUX__

	typedef long long           longlong;
	typedef long long           int64;
	typedef unsigned long long  uint64;
#endif
//-------------------------------------------------------------------------
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
//-------------------------------------------------------------------------
#ifndef ASSERT
#	ifdef _DEBUG
#		define ASSERT(expr)	{if (!(expr))__asm int 3}
#	else
#		define ASSERT(expr)	 ((void*)0)
#	endif
#endif
//-------------------------------------------------------------------------
#ifndef CCLOG
	#define FKLOG	FKSimpleLog
#else
	#define FKLOG	CCLOG
#endif
//-------------------------------------------------------------------------
#ifndef FK_OUT_PARAM
#define FK_OUT_PARAM
#endif
//-------------------------------------------------------------------------