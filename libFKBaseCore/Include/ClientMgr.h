//*************************************************************************
//	创建日期:	2014-8-6   14:06
//	文件名称:	ClientMgr.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "../../FKCommon/FKCommonDef.h"
#include "../../FKCommon/IFKClientSocket.h"
#include "../../FKCommon/DataStructure/TSwitchList.h"
#include <vector>
using std::vector;
//-------------------------------------------------------------------------
enum ENUM_ClientMgrConnectType
{
	eCMCT_Success		= 0,		// 连接成功
	eCMCT_Recv,						// 接收中
	eCMCT_ConnectError,				// 连接失败
	eCMCT_Close,					// 连接关闭
	eCMCT_MemError,					// 内存错误
	eCMCT_UnknownError,				// 未知错误
};
//-------------------------------------------------------------------------
struct SClientMgrCmd
{
	unsigned long			m_dwType;
	unsigned long			m_dwIndex;
	unsigned long			m_dwClientSocket;
	unsigned long			m_dwLen;
	vector<char>			m_vecData;

	SClientMgrCmd(){
		m_dwType = m_dwIndex = m_dwClientSocket = m_dwLen = 0;
	}

	SClientMgrCmd( const SClientMgrCmd& p_Other )
	{
		m_dwType		= p_Other.m_dwType;
		m_dwIndex		= p_Other.m_dwIndex;
		m_dwClientSocket= p_Other.m_dwClientSocket;
		m_dwLen			= p_Other.m_dwLen;
		m_vecData.clear();
		
		if( m_dwLen > 0 && m_dwLen <= MAX_RECVBUF_LEN )
		{
			m_vecData.resize( m_dwLen );
			if( m_vecData.capacity() >= m_dwLen )
			{
				std::copy( p_Other.m_vecData.begin(), p_Other.m_vecData.end(),
					m_vecData.begin() );
			}
			else
			{
				// 内存分配失败
				m_dwType = eCMCT_MemError;
				m_dwLen = 0;
			}
		}
		else
		{
			m_dwType = eCMCT_UnknownError;
			m_dwLen = 0;
		}
	}

	SClientMgrCmd& operator =( const SClientMgrCmd& p_Other )
	{
		m_dwType		= p_Other.m_dwType;
		m_dwIndex		= p_Other.m_dwIndex;
		m_dwClientSocket= p_Other.m_dwClientSocket;
		m_dwLen			= p_Other.m_dwLen;
		m_vecData.clear();

		if( m_dwLen > 0 && m_dwLen <= MAX_RECVBUF_LEN )
		{
			m_vecData.resize( m_dwLen );
			if( m_vecData.capacity() >= m_dwLen )
			{
				std::copy( p_Other.m_vecData.begin(), p_Other.m_vecData.end(),
					m_vecData.begin() );
			}
			else
			{
				// 内存分配失败
				m_dwType = eCMCT_MemError;
				m_dwLen = 0;
			}
		}
		else
		{
			m_dwType = eCMCT_UnknownError;
			m_dwLen = 0;
		}

		return *this;
	}
};
//-------------------------------------------------------------------------
class CClientMgr : public IClientMgr
{
public:
	CClientMgr();
	~CClientMgr();
public:
	// 创建
	virtual bool	Create();
	// 销毁
	virtual void	Release();

public:
	// 创建客户端socket
	virtual IClientSocket* CreateClientSocket( bool p_bEncrypt, IClientSocketSink* p_pClientSocketSink );
	// 清除客户端socket
	virtual void	ClearClientSocket( unsigned long p_dwIndex );

public:
	// 逻辑线程回调处理
	virtual void	OnEvent();
	// 设置客户端回调处理
	virtual void	SetLinkSink( unsigned long p_dwIndex, IClientSocketSink* p_pLinkSink );

public:
	// 连接成功
	void			PushConnectSuccess( unsigned long p_dwIndex, IClientSocket* p_pClientSocket );
	// 连接错误
	void			PushConnectError( unsigned long p_dwIndex, int p_nError, const char* p_szErrorMsg );
	// 接收数据
	void			PushRecv( unsigned long p_dwIndex, const char* p_szBuf, int p_nLen );
	// 连接关闭
	void			PushClose( unsigned long p_dwIndex );
private:
	// 链接成功处理
	void			__OnConnectSuccess( unsigned long p_dwIndex, IClientSocket* p_pClientSocket );
	// 链接错误处理
	void			__OnConnectError( unsigned long p_dwIndex, int p_nError, const char* p_szErrorMsg );
	// 接收数据处理
	void			__OnRecv( unsigned long p_dwIndex, const char* p_szBuf, int p_nLen );
	// 链接关闭处理
	void			__OnClose( unsigned long p_dwIndex );

private:
	unsigned long					m_dwIndex;				// 序号
	vector< IClientSocketSink* >	m_vecClientSocketSink;	// 回调函数列表
	TSwitchList< SClientMgrCmd >	m_listCmdList;			// 命令队列	
};