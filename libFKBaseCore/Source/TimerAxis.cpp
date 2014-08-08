//-------------------------------------------------------------------------
#include "../Include/TimerAxis.h"
//-------------------------------------------------------------------------
CTimerAxis::CTimerAxis()
{

}
//-------------------------------------------------------------------------
CTimerAxis::~CTimerAxis()
{

}
//-------------------------------------------------------------------------
// 创建
bool CTimerAxis::Create()
{
	try
	{
		m_TimerAxis.resize( (TIME_AXIS_LENGTH + TIME_GRID - 1) / TIME_GRID );
		m_dwInitTick = getTickCount();
		m_dwLastCheckTick = m_dwInitTick;
	}
	catch( ... )
	{
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------
// 销毁
void CTimerAxis::Release()
{
	try
	{
		for( unsigned long i = 0; i < m_TimerAxis.size(); ++i )
		{
			TIMER_LIST& TimerList = m_TimerAxis.at(i);
			TIMER_LIST::iterator Ite = TimerList.begin();
			for( ; Ite != TimerList.end(); ++Ite )
			{
				STimer* pTimer = *Ite;
				if( pTimer )
				{
					KillTimer( pTimer->m_dwTimerID, pTimer->m_pHandler );
				}
			}
		}

		m_TimerAxis.clear();
		delete this;
	}
	catch( ... )
	{
	}
}
//-------------------------------------------------------------------------
/**
@purpose				: 设置一个定时器
@param	 p_ulTimerID	: 定时器ID
@param   p_ulInterval	: 定时器调用间隔
@param   p_pHandler		: 处理接口
@param   p_ulCallTimes	: 调用次数,默认调用无穷次
@param   p_szDebugInfo	: 调试信息
@return					: 如果设置成功则返回true
*/
bool CTimerAxis::SetTimer( unsigned long p_ulTimerID, unsigned long p_ulInterval,
	ITimerHandler* p_pHandler, unsigned long p_ulCallTimes, const char* p_szDebugInfo )
{
	if( p_pHandler == NULL )
		return false;
	if( p_ulInterval == 0 )
	{
		p_ulInterval = 1;
	}
	void** ppTimerInfo = p_pHandler->GetTimerInfoPtr();
	if( ppTimerInfo == NULL )
		return false;

	TIMER_INFO* pTimerInfo = *(TIMER_INFO**)ppTimerInfo;
	if( pTimerInfo == NULL )
	{
		pTimerInfo = new TIMER_INFO;
		*ppTimerInfo = pTimerInfo;
	}

	// 检查是否已经添加了当前Timer
	TIMER_INFO::iterator Ite = pTimerInfo->begin();
	for( ; Ite != pTimerInfo->end(); ++Ite )
	{
		STimer& Timer = *Ite;
		if( Timer.m_dwTimerID == p_ulTimerID )
		{
			// 已经存在
			return false;
		}
	}

	STimer Timer;
	Timer.m_dwTimerID = p_ulTimerID;
	Timer.m_dwInterval = p_ulInterval;
	Timer.m_dwCallTimes = p_ulCallTimes;
	Timer.m_dwLastCallTick = m_dwLastCheckTick;
	Timer.m_pHandler = p_pHandler;
	Timer.m_szDebugInfo = p_szDebugInfo;
	Timer.m_dwGridIndex = ((Timer.m_dwLastCallTick + Timer.m_dwInterval - m_dwInitTick ) / TIME_GRID) % m_TimerAxis.size();

	pTimerInfo->push_back( Timer );

	STimer& TimerRef = pTimerInfo->back();
	STimer* TimerPtr = &TimerRef;
	m_TimerAxis[Timer.m_dwGridIndex].push_back( TimerPtr );
	TimerPtr->m_ItePos = --m_TimerAxis[Timer.m_dwGridIndex].end();

	return true;
}
//-------------------------------------------------------------------------

/**
@purpose				: 删除定时器
@param	 p_ulTimerID	: 定时器ID
@param   p_pHandler		: 处理接口
@return					: 返回是否删除成功
*/
bool CTimerAxis::KillTimer( unsigned long p_ulTimerID, ITimerHandler* p_pHandler )
{
	if( p_pHandler == NULL )
		return false;

	void** ppTimerInfo = p_pHandler->GetTimerInfoPtr();
	if( ppTimerInfo == NULL )
		return false;

	TIMER_INFO* pTimerInfo = *(TIMER_INFO**)ppTimerInfo;
	if( pTimerInfo == NULL )
		return false;

	// 遍历检查这个STimer
	TIMER_INFO::iterator Ite = pTimerInfo->begin();
	for( ; Ite != pTimerInfo->end(); ++Ite )
	{
		STimer& Timer = *Ite;
		if( Timer.m_dwTimerID == p_ulTimerID )
		{
			// 找到了
			// 注意：这里并未从时间轴内实际删除该对象，仅设置为 0 
			*Timer.m_ItePos = 0;

			pTimerInfo->erase( Ite );

			if( pTimerInfo->empty() )
			{
				delete pTimerInfo;
				*ppTimerInfo = 0;
			}

			return true;
		}
	}

	return false;
}
//-------------------------------------------------------------------------
// 每帧更新
void CTimerAxis::CheckTimer()
{
	unsigned long ulNow = getTickCount();
	// 不到16ms不进行更新检查
	if( ulNow - m_dwLastCheckTick < CHECK_FREQUENCY )
		return;

	unsigned long ulStartGrid = ((m_dwLastCheckTick - m_dwInitTick) / TIME_GRID ) % m_TimerAxis.size();
	unsigned long ulCurGrid = ((ulNow - m_dwInitTick) / TIME_GRID ) % m_TimerAxis.size();

	// 记录最后一次检查时间
	m_dwLastCheckTick = ulNow;

	// 遍历间隔时间之间的时间刻度
	unsigned long i = ulStartGrid;
	do 
	{
		TIMER_LIST& TimerList = m_TimerAxis[i];
		TIMER_LIST::iterator Ite = TimerList.begin();
		for( ; Ite != TimerList.end(); )
		{
			// 遍历时间刻度内的时间对象
			STimer* pTimer = *Ite;
			if( pTimer == NULL )
			{
				// 空的Timer对象，可能是KillTimer造成的空Timer，直接删除
				Ite = TimerList.erase( Ite );
				continue;
			}

			// 触发定时器
			if( ulNow - pTimer->m_dwLastCallTick >= pTimer->m_dwInterval )
			{
				pTimer->m_dwLastCallTick = ulNow;
				pTimer->m_dwCallTimes -= 1;

				// 执行回调
				pTimer->m_pHandler->OnTimer( pTimer->m_dwTimerID );

				if( pTimer->m_dwCallTimes <= 0 )
				{
					// 调用次数已满，可以删除了
					KillTimer( pTimer->m_dwTimerID, pTimer->m_pHandler );
				}
				else
				{
					// 搬迁到下一次触发的时间槽
					pTimer->m_dwGridIndex = (( pTimer->m_dwLastCallTick + pTimer->m_dwInterval - m_dwInitTick ) / TIME_GRID ) % m_TimerAxis.size();
					Ite = TimerList.erase( Ite );
					m_TimerAxis[ pTimer->m_dwGridIndex ].push_back( pTimer );
					pTimer->m_ItePos = --m_TimerAxis[ pTimer->m_dwGridIndex ].end();

					continue;
				}
			}

			// 没删除的直接迭代[ 被删除两种可能，一种是协助之前删除，一种是执行了之后删除 ]
			++Ite;
		}

		// 到达终点
		if( i == ulCurGrid )
		{
			break;
		}
		else
		{
			i = (i+1) % m_TimerAxis.size();
		}
	} while ( i != ulCurGrid );
}
//-------------------------------------------------------------------------