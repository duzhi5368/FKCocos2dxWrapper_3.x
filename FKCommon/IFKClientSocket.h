//*************************************************************************
//	创建日期:	2014-8-4   15:39
//	文件名称:	IFKClientSocket.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	客户端socket
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "FKCommonDef.h"
//-------------------------------------------------------------------------
// 推荐配置
//-------------------------------------------------------------------------
// 客户端最大socket数量
#define MAX_CLIENT_SOCKET_NUM	16
// 单个发送/接收消息最大长度
#define MAX_RECVBUF_LEN			8192
#define MAX_SENDBUF_LEN			8192
//-------------------------------------------------------------------------
struct IClientSocket;
// 客户端连接回调处理
struct IClientSocketSink
{
	// 客户端接收数据
	virtual void	OnRecv( const char* p_szBuf, int32 p_nLen ) = 0;
	// 客户端连接关闭
	virtual void	OnCloseConnect() = 0;
	// 客户端连接成功
	virtual void	OnConnectOK( IClientSocket* p_pClientSocket ) = 0;
	// 客户端连接错误
	virtual void	OnConnectError( int p_nSocketID, int p_nErrorCode, const char* p_szErrorMsg ) = 0;
};
//-------------------------------------------------------------------------
// 客户端连接socket
struct IClientSocket
{
	// 释放
	virtual void	Release() = 0;
	
public:
	// 设置客户端回调处理
	virtual void	SetLinkSink( IClientSocketSink* p_pLinkSink ) = 0;
	// 连接服务器
	virtual bool	Connect( const char* p_szServerIP, uint16 p_usServerPort ) = 0;
	// 主动关闭连接
	virtual void	CloseConnect() = 0;
	// 发送数据
	virtual bool	Send( const char* p_szBuf, int32 p_nLen ) = 0;
	// 获取socketID
	virtual int32	GetSocketID() = 0;
	// 获取客户端状态
	virtual bool	IsConnect() = 0;
};
//-------------------------------------------------------------------------
// 客户端管理器
struct IClientMgr
{
	// 创建
	virtual bool	Create() = 0;
	// 销毁
	virtual void	Release() = 0;

public:
	// 创建客户端socket
	virtual IClientSocket* CreateClientSocket( bool p_bEncrypt, IClientSocketSink* p_pClientSocketSink ) = 0;
	// 清除客户端socket
	virtual void	ClearClientSocket( unsigned long p_dwIndex ) = 0;

public:
	// 逻辑线程回调处理
	virtual void	OnEvent() = 0;
	// 设置客户端回调处理
	virtual void	SetLinkSink( unsigned long p_dwIndex, IClientSocketSink* p_pLinkSink ) = 0;
};