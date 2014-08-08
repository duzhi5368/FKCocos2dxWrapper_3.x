//*************************************************************************
//	创建日期:	2014-8-7   14:27
//	文件名称:	TimerAxis.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	时间轴
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "../../FKCommon/IFKTimerAxis.h"
#include <string>
#include <list>
#include <vector>
using std::string;
using std::list;
using std::vector;
//-------------------------------------------------------------------------
class CTimerAxis : public ITimerAxis
{
public:
	CTimerAxis();
	~CTimerAxis();
public:
		// 创建
	virtual bool	Create();
	// 销毁
	virtual void	Release();

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
		const char* p_szDebugInfo = NULL );

	/**
	@purpose				: 删除定时器
	@param	 p_ulTimerID	: 定时器ID
	@param   p_pHandler		: 处理接口
	@return					: 返回是否删除成功
	*/
	virtual bool	KillTimer( unsigned long p_ulTimerID, ITimerHandler* p_pHandler );

	// 每帧更新
	virtual void	CheckTimer();

protected:
	struct STimer
	{
		unsigned long		m_dwTimerID;
		unsigned long		m_dwInterval;		// 间隔时间
		unsigned long		m_dwCallTimes;		// 累计调用次数
		unsigned long		m_dwLastCallTick;	// 上次调用时间
		unsigned long		m_dwGridIndex;		// 所在时间刻度
		ITimerHandler*		m_pHandler;			// 回调函数
		string				m_szDebugInfo;		// 调试信息
		list<STimer*>::iterator	m_ItePos;		// 记录自己在时间轴中的Ite，加速搜索
	};

	typedef list<STimer>		TIMER_INFO;		// 在ITimerHandler中保存的定时器完整信息
	typedef list<STimer*>		TIMER_LIST;		// 在时间刻度中存放的定时器列表
	typedef vector< TIMER_LIST > TIMER_AXIS;	// 保存时间刻度的一个时间轴

	TIMER_AXIS				m_TimerAxis;		// 时间轴对象
	unsigned long			m_dwLastCheckTick;	// 上一次检查的时间
	unsigned long			m_dwInitTick;		// 初始化时间轴时间
};
//-------------------------------------------------------------------------