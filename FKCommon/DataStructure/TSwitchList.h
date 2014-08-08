//*************************************************************************
//	创建日期:	2014-8-6   15:08
//	文件名称:	TSwitchList.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include <list>
#include <mutex>
using std::list;
using std::mutex;
//-------------------------------------------------------------------------
template< class SCommand >
class TSwitchList
{
public:
	typedef list<SCommand>		COMMAND_LIST;		// 命令列表
private:
	COMMAND_LIST*				m_pCurrentInList;	// 当前输入列表
	COMMAND_LIST*				m_pCurrentOutList;	// 当前输出列表
	mutex						m_hLock;			// 锁
	COMMAND_LIST				m_tagList1;
	COMMAND_LIST				m_tagList2;
public:
	TSwitchList()
		: m_pCurrentInList( &m_tagList1 )
		, m_pCurrentOutList( &m_tagList2 )
	{
		
	}
	
	virtual ~TSwitchList(){

	}

	void PushFront( SCommand* p_pCmd )
	{
		if( p_pCmd == NULL ) 
			return;

		m_hLock.lock();
		m_pCurrentInList->push_front( *p_pCmd );
		m_hLock.unlock();
	}

	void Push( SCommand* p_pCmd )
	{
		if( p_pCmd == NULL )
			return;

		m_hLock.lock();
		m_pCurrentInList->push_back( *p_pCmd );
		m_hLock.unlock();
	}

	// 翻转
	bool SwitchState()
	{
		m_hLock.lock();

		if( m_pCurrentOutList->size() > 0 )
		{
			m_hLock.unlock();
			return false;
		}

		COMMAND_LIST* pList = m_pCurrentInList;
		m_pCurrentInList = m_pCurrentOutList;
		m_pCurrentOutList = pList;

		m_hLock.unlock();
		return true;
	}

	COMMAND_LIST& GetOutList()
	{
		return *m_pCurrentOutList;
	}
};