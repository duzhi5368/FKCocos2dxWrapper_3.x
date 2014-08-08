//*************************************************************************
//	创建日期:	2014-8-6   16:19
//	文件名称:	TSemaphore.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#if defined(linux) || defined(SOLARIS)
	#include <semaphore.h>
	#define SEM_CREATE(p,s) sem_init(&(p), 1, s)
	#define SEM_SIGNAL(p)   sem_post(&(p))
	#define SEM_WAIT(p)     sem_wait(&(p))
	#define SEM_CLOSE(p)    sem_destroy(&(p))
	typedef sem_t           semaphore;
#elif defined(WIN32)
	#include <windows.h>
	#define SEM_CREATE(p,s) (!(p = CreateSemaphore(NULL, (long)(s), (long)(s), NULL)))
	#define SEM_SIGNAL(p)   (!ReleaseSemaphore(p, 1, NULL))
	#define SEM_WAIT(p)     WaitForSingleObject(p, INFINITE)
	#define SEM_CLOSE(p)    (!CloseHandle(p))
	typedef HANDLE          semaphore;
#elif defined(OS2)
	#include "os2_semaphore.h"
	#define SEM_CREATE(p,s) sem_init(&(p), 1, s)
	#define SEM_SIGNAL(p)   sem_post(&(p))
	#define SEM_WAIT(p)     sem_wait(&(p))
	#define SEM_CLOSE(p)    sem_destroy(&(p))
	typedef sem_t           semaphore;
#elif defined(__APPLE__)
	#include <Carbon/Carbon.h>
	#define SEM_CREATE(p,s) MPCreateSemaphore(s, s, &(p))
	#define SEM_SIGNAL(p)   MPSignalSemaphore(p)
	#define SEM_WAIT(p)     MPWaitOnSemaphore(p, kDurationForever)
	#define SEM_CLOSE(p)    MPDeleteSemaphore(p)
	typedef MPSemaphoreID   semaphore;
#elif defined(TARGET_OS_IPHONE)
	#include <semaphore.h>
	#define SEM_CREATE(p,s) (p = sem_open("FKAsync", O_CREAT, 0644, s))
	#define SEM_SIGNAL(p)   sem_post(&(p))
	#define SEM_CLOSE(p)	sem_unlink("FKAsync"); sem_close(&(p))
	#define SEM_WAIT(p)     sem_wait(&(p))
	typedef sem_t           semaphore;
#endif
//-------------------------------------------------------------------------