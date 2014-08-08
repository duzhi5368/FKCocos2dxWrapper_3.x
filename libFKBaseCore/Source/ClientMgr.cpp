//-------------------------------------------------------------------------
#include "../include/ClientMgr.h"
#include "../include/ClientSocket.h"
#include "../include/TCPWarpper.h"
#include "../../FKCommon/BaseComponent/DataArchive.h"
//-------------------------------------------------------------------------
CClientMgr::CClientMgr()
	: m_dwIndex( 0 )
{
	m_vecClientSocketSink.resize(MAX_CLIENT_SOCKET_NUM);
	memset( &m_vecClientSocketSink[0], 0, MAX_CLIENT_SOCKET_NUM * sizeof(IClientSocketSink*) );
}
//-------------------------------------------------------------------------
CClientMgr::~CClientMgr()
{

}
//-------------------------------------------------------------------------
// 创建
bool CClientMgr::Create()
{
	// 初始化套接字
	int nRet = init_socket();
	if( nRet != 0 )
	{
		FKLOG("Error : Init socket failed %d", nRet );
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------
// 销毁
void CClientMgr::Release()
{
	m_vecClientSocketSink.clear();
	cleanup_socket();
}
//-------------------------------------------------------------------------
// 创建客户端socket
IClientSocket* CClientMgr::CreateClientSocket( bool p_bEncrypt, IClientSocketSink* p_pClientSocketSink )
{
	if( p_pClientSocketSink == NULL || m_dwIndex >= MAX_CLIENT_SOCKET_NUM )
		return NULL;

	CClientSocket* pClientSocket = new CClientSocket();
	if( pClientSocket == NULL )
		return NULL;

	if( !pClientSocket->Create( m_dwIndex, this ) )
	{
		delete pClientSocket;
		return NULL;
	}

	m_vecClientSocketSink[m_dwIndex++] = p_pClientSocketSink;
	return pClientSocket;
}
//-------------------------------------------------------------------------
// 清除客户端socket
void CClientMgr::ClearClientSocket( unsigned long p_dwIndex )
{
	if( p_dwIndex >= MAX_CLIENT_SOCKET_NUM )
		return;
	m_vecClientSocketSink[p_dwIndex] = NULL;
}
//-------------------------------------------------------------------------
// 逻辑线程回调处理
void CClientMgr::OnEvent()
{

}
//-------------------------------------------------------------------------
// 设置客户端回调处理
void CClientMgr::SetLinkSink( unsigned long p_dwIndex, IClientSocketSink* p_pLinkSink )
{
	if( p_dwIndex >= MAX_CLIENT_SOCKET_NUM )
		return;
	m_vecClientSocketSink[p_dwIndex] = p_pLinkSink;
}
//-------------------------------------------------------------------------
// 连接成功
void CClientMgr::PushConnectSuccess( unsigned long p_dwIndex, IClientSocket* p_pClientSocket )
{
	SClientMgrCmd tagCmd;
	tagCmd.m_dwType = eCMCT_Success;
	tagCmd.m_dwIndex = p_dwIndex;
	tagCmd.m_dwClientSocket = ( unsigned long )p_pClientSocket;

	m_listCmdList.Push( &tagCmd );
}
//-------------------------------------------------------------------------
// 连接错误
void CClientMgr::PushConnectError( unsigned long p_dwIndex, int p_nError, const char* p_szErrorMsg )
{
	SClientMgrCmd tagCmd;
	tagCmd.m_dwType = eCMCT_ConnectError;
	tagCmd.m_dwIndex = p_dwIndex;
	tagCmd.m_dwClientSocket = p_nError;
	tagCmd.m_dwLen = (int)( strlen(p_szErrorMsg) + 1 );
	tagCmd.m_vecData.resize( tagCmd.m_dwLen );

	if( (int)tagCmd.m_vecData.capacity() >= (int)(strlen(p_szErrorMsg)) )
	{
		// 成功
		memcpy( &tagCmd.m_vecData[0], p_szErrorMsg, strlen(p_szErrorMsg) );
	}
	else
	{
		return;
	}

	m_listCmdList.Push( &tagCmd );
}
//-------------------------------------------------------------------------
// 接收数据
void CClientMgr::PushRecv( unsigned long p_dwIndex, const char* p_szBuf, int p_nLen )
{
	if( p_szBuf == NULL || p_nLen <= 0 || p_nLen > MAX_RECVBUF_LEN )
		return;

	SClientMgrCmd tagCmd;
	tagCmd.m_dwType = eCMCT_Recv;
	tagCmd.m_dwIndex = p_dwIndex;
	tagCmd.m_dwLen = p_nLen;
	tagCmd.m_vecData.resize( p_nLen );

	if( (int)tagCmd.m_vecData.capacity() >= p_nLen )
	{
		// 成功
		memcpy( &tagCmd.m_vecData[0], p_szBuf, p_nLen );
	}
	else
	{
		return;
	}

	m_listCmdList.Push( &tagCmd );
}
//-------------------------------------------------------------------------
// 连接关闭
void CClientMgr::PushClose( unsigned long p_dwIndex )
{
	SClientMgrCmd tagCmd;
	tagCmd.m_dwType = eCMCT_Close;
	tagCmd.m_dwIndex = p_dwIndex;

	m_listCmdList.Push( &tagCmd );
}
//-------------------------------------------------------------------------
// 链接成功处理
void CClientMgr::__OnConnectSuccess( unsigned long p_dwIndex, IClientSocket* p_pClientSocket )
{

}
//-------------------------------------------------------------------------
// 链接错误处理
void CClientMgr::__OnConnectError( unsigned long p_dwIndex, int p_nError, const char* p_szErrorMsg )
{

}
//-------------------------------------------------------------------------
// 接收数据处理
void CClientMgr::__OnRecv( unsigned long p_dwIndex, const char* p_szBuf, int p_nLen )
{

}
//-------------------------------------------------------------------------
// 链接关闭处理
void CClientMgr::__OnClose( unsigned long p_dwIndex )
{

}
//-------------------------------------------------------------------------