//*************************************************************************
//	创建日期:	2014-8-4   15:51
//	文件名称:	IFKTimerAxis.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "FKCommonDef.h"
//-------------------------------------------------------------------------
#ifndef _WIN32
#include <sys/time.h>
#endif
//-------------------------------------------------------------------------
// 推荐配置
//-------------------------------------------------------------------------
// 检查频率 16 ms
#define CHECK_FREQUENCY			16
// 时间刻度单位 64 ms
#define TIME_GRID				64
// 时间轴总长度 ms [2分钟]
#define TIME_AXIS_LENGTH		720000
// 无效的定时器ID
#define INVALID_TIMER			0xffffffff
// 定时器回调次数 无穷次
#define INFINITY_CALL			0xffffffff
//-------------------------------------------------------------------------
// 定时器回调接口
struct ITimerHandler
{
	ITimerHandler()
		: m_pInfoPtr( NULL )
	{

	}
	// 定时器触发回调
	virtual void	OnTimer( uint32 p_unEventID ) = 0;
	/**
	@purpose          : 取得存放定时器内部数据的指针
	@note             : 这个函数和应用层无关,参照TimerHandler基类的实现即可
	@note             : 时间轴具体实现中需要提高定时器的插入删除效率，所以让应用层记录一个指针
	@return		      : 返回指向临时信息的指针,该指针必须初始化成0
	*/
	virtual void**	GetTimerInfoPtr(){
		return &m_pInfoPtr;
	}

protected:
	void*	m_pInfoPtr;		// 指向临时数据的指针
};
//-------------------------------------------------------------------------
// 时间轴
struct ITimerAxis
{
	// 创建
	virtual bool	Create() = 0;
	// 销毁
	virtual void	Release() = 0;

public:
	/**
	@purpose				: 设置一个定时器
	@param	 p_ulTimerID	: 定时器ID
	@param   p_ulInterval	: 定时器调用间隔
	@param   p_pHandler		: 处理接口
	@param   p_ulCallTimes	: 调用次数,默认调用无穷次
	@param   p_szDebugInfo	: 调试信息
	@return					: 如果设置成功则返回true
	*/
	virtual bool	SetTimer( unsigned long p_ulTimerID, unsigned long p_ulInterval,
		ITimerHandler* p_pHandler, unsigned long p_ulCallTimes = INFINITY_CALL,
		const char* p_szDebugInfo = NULL ) = 0;

	/**
	@purpose				: 删除定时器
	@param	 p_ulTimerID	: 定时器ID
	@param   p_pHandler		: 处理接口
	@return					: 返回是否删除成功
	*/
	virtual bool	KillTimer( unsigned long p_ulTimerID, ITimerHandler* p_pHandler ) = 0;

	// 获取当前时间
	inline unsigned long getTickCount() 
	{
#ifdef	_WIN32
		return ::GetTickCount();
#else
		struct timeval tv;
		if(gettimeofday(&tv,NULL)!=0)
			return 0;
		return (tv.tv_sec*1000) + (tv.tv_usec/1000);
#endif
	}
};
//-------------------------------------------------------------------------