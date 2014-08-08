//-------------------------------------------------------------------------
#include "../include/ClientSocket.h"
#include "../Include/ClientMgr.h"
#include "../include/TCPWarpper.h"
//-------------------------------------------------------------------------
// 客户端IO主处理函数
static void* ClientIOThreadProc( void* p_pClientSocket )
{
	CClientSocket* pClientSocket = (CClientSocket*)p_pClientSocket;
	if( pClientSocket == NULL )
		return NULL;

	int nSocket = pClientSocket->GetSocket();
	unsigned long dwEventIndex = 0;

	while( !pClientSocket->IsExit() )
	{
		if( !pClientSocket->OnWSAEvent(nSocket) )
		{
			return NULL;
		}
	}
	return NULL;
}
//-------------------------------------------------------------------------
CClientSocket::CClientSocket()
	: m_bIsExit( false )
	, m_pClientMgr( NULL )
	, m_dwIndex( 0 )
	, m_nSocket( INVALID_SOCKET )
	, m_nRecvLeaveLen( 0 )
	, m_bIsConnect( false )
	, m_pIOThread( NULL )
{
	memset( m_szBuffer, 0, sizeof(m_szBuffer) );
}
//-------------------------------------------------------------------------
CClientSocket::~CClientSocket()
{
	// 停止客户端线程
	if( m_pIOThread != NULL )
	{
		m_bIsExit = true;
		m_pIOThread->join();
		m_pIOThread->detach();
		// 是否要delete还值得商榷
		delete m_pIOThread;
		m_pIOThread = NULL;
	}

	// 清理socket
	ClearSocket();

	// 从mgr移除
	if( m_pClientMgr != NULL )
	{
		m_pClientMgr->ClearClientSocket( m_dwIndex );
		m_pClientMgr = NULL;
	}
}
//-------------------------------------------------------------------------
// 创建单个客户端socket
bool CClientSocket::Create( unsigned long p_dwIndex, CClientMgr* p_pClientMgr )
{
	if( p_pClientMgr == NULL )
		return false;

	m_pClientMgr = p_pClientMgr;
	m_dwIndex = p_dwIndex;

	return true;
}
//-------------------------------------------------------------------------
// 释放
void CClientSocket::Release()
{
	CloseConnect();
	delete this;
}
//-------------------------------------------------------------------------
// 设置客户端回调处理
void CClientSocket::SetLinkSink( IClientSocketSink* p_pLinkSink )
{
	if( m_pClientMgr )
	{
		m_pClientMgr->SetLinkSink( m_dwIndex, p_pLinkSink );
	}
}
//-------------------------------------------------------------------------
// 连接服务器
bool CClientSocket::Connect( const char* p_szServerIP, uint16 p_usServerPort )
{
	FKLOG("try to connect server: ip = %s, port = %d", p_szServerIP, p_usServerPort );
	// 创建TCP Socket
	m_nSocket = create_tcp_socket();

	// 设置接受缓冲
	int nRecvBufSize = 65535;
	if( setsockopt( m_nSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nRecvBufSize, sizeof(int) ))
	{
		close_tcp_socket( m_nSocket );
		m_nSocket = INVALID_SOCKET;
		FKLOG("Close socket: ip = %s, port = %d", p_szServerIP, p_usServerPort );
		return false;
	}

	// 设置发送缓冲
	int nSendBufSize = 65535;
	if( setsockopt( m_nSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nSendBufSize, sizeof(int) ))
	{
		close_tcp_socket( m_nSocket );
		m_nSocket = INVALID_SOCKET;
		FKLOG("Close socket: ip = %s, port = %d", p_szServerIP, p_usServerPort );
		return false;
	}

	// 设置禁止使用Nagle合并包
	bool bNoDelay = true;
	if( setsockopt( m_nSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bool) ))
	{
		close_tcp_socket( m_nSocket );
		m_nSocket = INVALID_SOCKET;
		FKLOG("Close socket: ip = %s, port = %d", p_szServerIP, p_usServerPort );
		return false;
	}

	int nTimeOut = 5000;
	// 设置接收和发送超时
	if( setsockopt( m_nSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTimeOut, sizeof(int)))
	{
		close_tcp_socket( m_nSocket );
		m_nSocket = INVALID_SOCKET;
		FKLOG("Close socket: ip = %s, port = %d", p_szServerIP, p_usServerPort );
		return false;
	}
	if( setsockopt( m_nSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&nTimeOut, sizeof(int)))
	{
		close_tcp_socket( m_nSocket );
		m_nSocket = INVALID_SOCKET;
		FKLOG("Close socket: ip = %s, port = %d", p_szServerIP, p_usServerPort );
		return false;
	}

	// 真实连接
	int nRet = connect_server( m_nSocket, (char*)p_szServerIP, p_usServerPort );
	if( nRet != 0 )
	{
		close_tcp_socket( m_nSocket );
		m_nSocket = INVALID_SOCKET;
		FKLOG("Connect Server failed! Close socket: ip = %s, port = %d", p_szServerIP, p_usServerPort );
		return false;
	}
	
	// 连接成功,创建线程接收

	m_pIOThread = new thread( ClientIOThreadProc, (void*)this );
	if( m_pIOThread == NULL )
	{
		close_tcp_socket( m_nSocket );
		m_nSocket = INVALID_SOCKET;
		FKLOG("Connect Server failed! Close socket: ip = %s, port = %d", p_szServerIP, p_usServerPort );
		return false;
	}

	// 回调
	OnConnectSuccess();

	// 记录
	strcpy_s( m_szServerIP, 18, p_szServerIP );
	m_usServerPort =  p_usServerPort;

	FKLOG("connect server success: ip = %s, port = %d", p_szServerIP, p_usServerPort );

	return true;
}
//-------------------------------------------------------------------------
// 主动关闭连接
void CClientSocket::CloseConnect()
{
	m_bIsConnect = false;

	ClearSocket();

	if( m_pClientMgr )
	{
		m_pClientMgr->PushClose( m_dwIndex );
	}
}
//-------------------------------------------------------------------------
// 发送数据
bool CClientSocket::Send( const char* p_szBuf, int32 p_nLen )
{
	if( p_szBuf == NULL || p_nLen < 0 || p_nLen >= MAX_SENDBUF_LEN )
		return false;

	m_ClientSendThread.PushSend( p_szBuf, p_nLen );
	return true;
}
//-------------------------------------------------------------------------
// 获取socketID
int32 CClientSocket::GetSocketID()
{
	return m_dwIndex;
}
//-------------------------------------------------------------------------
// 获取客户端状态
bool CClientSocket::IsConnect()
{
	return m_bIsConnect;
}
//-------------------------------------------------------------------------
// WSA事件处理
bool CClientSocket::OnWSAEvent( int p_nSocket )
{
	fd_set	tagFDSet;

	// 监控文件描述符集合中最大的文件号
	int		nMaxSocket = p_nSocket;
	// 初始化文件描述符集合
	FD_ZERO( &tagFDSet );
	FD_SET( m_nSocket, &tagFDSet );

	/*
	//Select 函数原型
	//			int select（nfds， readfds， writefds， exceptfds， timeout）
	//nfds:		select监视的文件句柄数,视进程中打开的文件数而定，一般设为要监视各文件中的最大文件号加一
	//readfds:	select监视的可读文件句柄集合
	//writefds:	select监视的可写文件句柄集合。
	//exceptfds:select监视的异常文件句柄集合。
	//timeout:	本次select的超时结束时间。
	*/
	int nRet = select( nMaxSocket + 1, &tagFDSet, NULL, NULL, NULL );
	if( FD_ISSET( p_nSocket, &tagFDSet ) )
	{
		m_nRecvBytes = recv( p_nSocket, m_szRecvBuf, sizeof( m_szRecvBuf ), 0 );
		if( m_nRecvBytes > 0 )
		{
			OnRecv();
		}
		else if( m_nRecvBytes == -1 )
		{
			if( m_bIsConnect )
			{
				CloseConnect();
			}
			return false;
		}
	}
	return true;
}
//-------------------------------------------------------------------------
// 链接成功
bool CClientSocket::OnConnectSuccess()
{
	if( !m_ClientSendThread.Start( m_nSocket ) )
	{
		return false;
	}

	m_bIsConnect = true;

	if( m_pClientMgr )
	{
		m_pClientMgr->PushConnectSuccess( m_dwIndex, this );
	}
	return true;
}
//-------------------------------------------------------------------------
// 连接失败
bool CClientSocket::OnConnectError( const char* p_szErrorMsg, int p_nErrorCode )
{
	m_bIsConnect = false;
	ClearSocket();

	if( m_pClientMgr != NULL )
	{
		m_pClientMgr->PushConnectError( m_dwIndex, p_nErrorCode, p_szErrorMsg );
	}
	return true;
}
//-------------------------------------------------------------------------
// 接收
bool CClientSocket::OnRecv()
{
	char* pBuf = m_szRecvBuf;
	int nLen = m_nRecvBytes;

	if( pBuf == NULL || nLen <= 0 )
		return false;

	int nBufOffset = 0;		// 偏移量
	uint16 usCmdLen = 0;

	// 上次接收包有剩余的未处理完
	if( m_nRecvLeaveLen > 0 )
	{
		if( m_nRecvLeaveLen == sizeof(unsigned char) )
		{
			nBufOffset = 1;	// 补齐一个字节
			memcpy( m_szBuffer + m_nRecvLeaveLen, pBuf, nBufOffset );

			pBuf += nBufOffset;
			nLen -= nBufOffset;
			m_nRecvLeaveLen += nBufOffset;
		}

		// 等待下次接收
		if( nLen <= 0 )
		{
			return false;
		}

		if( m_nRecvLeaveLen >= sizeof(uint16) )
		{
			// 剩余长度至少有一个 uint16 长
			usCmdLen = *((uint16*) m_szBuffer );

			if( usCmdLen > MAX_RECVBUF_LEN )
			{
				// 长度非法，关闭
				CloseConnect();
				return false;
			}

			// 否则开始处理
			if( m_nRecvLeaveLen >= (int)(usCmdLen + sizeof(uint16)) )
			{
				// 完整包
				nBufOffset = usCmdLen - m_nRecvLeaveLen + sizeof( uint16 );	// 上一个包长
				memcpy( m_szBuffer + m_nRecvLeaveLen, pBuf, nBufOffset );

				if( !ProcFullPackage(&m_szBuffer[2], usCmdLen) )
				{
					return false;
				}

				pBuf += nBufOffset;
				nLen -= nBufOffset;
				m_nRecvLeaveLen = 0;
			}
			else
			{
				// 部分包，不完整，先保存起来
				memcpy( m_szBuffer + m_nRecvLeaveLen, pBuf, nLen );
				m_nRecvLeaveLen += nLen;
				return true;
			}
		}
	}

	// 接收出错
	if( m_nRecvLeaveLen < 0 )
	{
		CloseConnect();
		return false;
	}

	// 第一次接收
	while( true )
	{
		if( nLen <= 0 )
		{
			// 等待下次接收
			return true;
		}

		if( nLen >= sizeof(uint16) )
		{
			usCmdLen = *((uint16*)pBuf);

			if( usCmdLen > MAX_RECVBUF_LEN )
			{
				// 长度非法，关闭
				CloseConnect();
				return false;
			}

			if( nLen >= usCmdLen )
			{
				// 多于一个完整包
				if( !ProcFullPackage((unsigned char*)pBuf, usCmdLen ) )
				{
					return false;
				}

				pBuf += usCmdLen;
				nLen -= usCmdLen;
			}
			else
			{
				// 不完整的包，保存起来
				memcpy( m_szBuffer, pBuf, nLen );
				m_nRecvLeaveLen = nLen;
				return true;
			}
		}
		else
		{
			// 只有一个字节，保存，等待下次接收
			memcpy( m_szBuffer, pBuf, nLen );
			m_nRecvLeaveLen = nLen;
			return true;
		}
	}

	return true;
}
//-------------------------------------------------------------------------
// 关闭消息
bool CClientSocket::OnClose()
{
	CloseConnect();
	return true;
}
//-------------------------------------------------------------------------
// 处理完整接收包
bool CClientSocket::ProcFullPackage( unsigned char* p_szBuf, uint16 p_usLen )
{
	if( m_pClientMgr == NULL )
		return true;

	m_pClientMgr->PushRecv( m_dwIndex, (const char*)p_szBuf, p_usLen );
	return true;
}
//-------------------------------------------------------------------------
// 获取socket
int CClientSocket::GetSocket()
{
	return m_nSocket;
}
//-------------------------------------------------------------------------
// 清除socket
void CClientSocket::ClearSocket()
{
	if( m_nSocket > 0 && m_nSocket != INVALID_SOCKET )
	{
		close_tcp_socket( m_nSocket );
	}
	m_ClientSendThread.Stop();
}
//-------------------------------------------------------------------------
// 是否退出
bool CClientSocket::IsExit()
{
	return m_bIsExit;
}
//-------------------------------------------------------------------------