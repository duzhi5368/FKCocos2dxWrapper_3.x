//*************************************************************************
//	创建日期:	2014-8-6   15:35
//	文件名称:	ClientSendThread.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "../../FKCommon/FKCommonDef.h"
#include "../../FKCommon/IFKClientSocket.h"
#include "../../FKCommon/DataStructure/TSwitchList.h"
#include "../../FKCommon/BaseComponent/TSemaphore.h"
#include <vector>
#include <thread>
using std::vector;
using std::thread;
//-------------------------------------------------------------------------
struct SSendDataPack
{
	unsigned long		m_dwLen;		// 长度
	vector<char>		m_vecData;		// 数据

	SSendDataPack() 
		:m_dwLen ( 0 )
	{

	}

	SSendDataPack( const SSendDataPack& p_Other )
	{
		m_dwLen = p_Other.m_dwLen;
		m_vecData.clear();

		if( m_dwLen > 0 && m_dwLen <= MAX_SENDBUF_LEN )
		{
			m_vecData.resize( m_dwLen );
			if( m_vecData.capacity() >= m_dwLen )
			{
				std::copy( p_Other.m_vecData.begin(),
					p_Other.m_vecData.end(), m_vecData.begin() );
			}
			else
			{
				m_dwLen = 0;
			}
		}
	}

	SSendDataPack& operator = ( const SSendDataPack& p_Other )
	{
		m_dwLen = p_Other.m_dwLen;
		m_vecData.clear();

		if( m_dwLen > 0 && m_dwLen <= MAX_SENDBUF_LEN )
		{
			m_vecData.resize( m_dwLen );
			if( m_vecData.capacity() >= m_dwLen )
			{
				std::copy( p_Other.m_vecData.begin(),
					p_Other.m_vecData.end(), m_vecData.begin() );
			}
			else
			{
				m_dwLen = 0;
			}
		}

		return *this;
	}
};
//-------------------------------------------------------------------------
class CClientSendThread
{
public:
	CClientSendThread();
	~CClientSendThread();
public:
	// 启动
	bool	Start( int p_nSocketID );
	// 停止
	void	Stop();
public:
	// 请求发送
	bool	PushSend( const char* p_szBuf, int p_nLen );
	// 收到发送事件
	void	OnSendEvent();
	// 真实发送
	void	OnSend( const char* p_szBuf, int p_nLen );
	// 获取信号器
	semaphore*	GetSemphore();
	// 判断是否结束退出
	bool	IsExit();
private:
	bool						m_bEncrypt;				// 是否加密
	int							m_nSocketID;			// socket
	bool						m_bIsExit;				// 是否退出
	thread*						m_pThreadHandle;		// 线程对象
	bool						m_bIsHadThread;			// 线程是否创建
	semaphore*					m_pSemphore;			// 信号量
	char						m_szTmpSendBuf[MAX_SENDBUF_LEN+2];	// 发送缓冲
	TSwitchList<SSendDataPack>	m_WaitingSendList;		// 等待发送队列
};	
//-------------------------------------------------------------------------