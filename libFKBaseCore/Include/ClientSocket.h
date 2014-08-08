//*************************************************************************
//	创建日期:	2014-8-6   18:05
//	文件名称:	ClientSocket.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "../../FKCommon/IFKClientSocket.h"
#include "ClientSendThread.h"
#include <thread>
using std::thread;
//-------------------------------------------------------------------------
enum ENUM_SocketConnectState
{
	eSCS_Connnecting	=	0,	// 正在链接
	eSCS_Connected,				// 已链接
	eSCS_ConnectClosed,			// 链接已完毕
};
//-------------------------------------------------------------------------
class CClientMgr;
class CClientSocket : public IClientSocket
{
public:
	CClientSocket();
	~CClientSocket();
public:
	// 创建单个客户端socket
	bool			Create( unsigned long p_dwIndex, CClientMgr* p_pClientMgr );
	// 释放
	virtual void	Release();

public:
	// 设置客户端回调处理
	virtual void	SetLinkSink( IClientSocketSink* p_pLinkSink );
	// 连接服务器
	virtual bool	Connect( const char* p_szServerIP, uint16 p_usServerPort );
	// 主动关闭连接
	virtual void	CloseConnect();
	// 发送数据
	virtual bool	Send( const char* p_szBuf, int32 p_nLen );
	// 获取socketID
	virtual int32	GetSocketID();
	// 获取客户端状态
	virtual bool	IsConnect();
	// WSA事件处理
	bool			OnWSAEvent( int p_nSocket );
	// 链接成功
	bool			OnConnectSuccess();
	// 连接失败
	bool			OnConnectError( const char* p_szErrorMsg, int p_nErrorCode );
	// 接收
	bool			OnRecv();
	// 关闭消息
	bool			OnClose();
	// 处理完整接收包
	bool			ProcFullPackage( unsigned char* p_szBuf, uint16 p_usLen );
	// 获取socket
	int				GetSocket();
	// 清除socket
	void			ClearSocket();
	// 是否退出
	bool			IsExit();
private:
	bool				m_bIsExit;			// 是否结束
	unsigned long		m_dwIndex;			// socketID
	CClientMgr*			m_pClientMgr;		// 客户端管理器
	CClientSendThread	m_ClientSendThread;	// 客户端发送线程
	int					m_nSocket;			// 客户端socket
	int					m_nRecvBytes;		// 接收长度
	int					m_nRecvLeaveLen;	// 接收剩余长度
	unsigned char		m_szBuffer[MAX_RECVBUF_LEN];	// 组包缓冲
	char				m_szRecvBuf[MAX_RECVBUF_LEN];	// 单个接收缓冲
	void*				m_hThread;			// 线程句柄
	bool				m_bIsConnect;		// 是否已链接
	thread*				m_pIOThread;		// IO加载线程
	char				m_szServerIP[18];	// 服务器IP
	uint16				m_usServerPort;		// 服务器端口
};
//-------------------------------------------------------------------------