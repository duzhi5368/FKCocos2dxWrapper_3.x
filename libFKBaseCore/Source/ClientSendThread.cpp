//-------------------------------------------------------------------------
#include "../include/ClientSendThread.h"
#include "../include/TCPWarpper.h"
#include "../../FKCommon/IFKClientSocket.h"
#include "../../FKCommon/IFKBaseCore.h"
//-------------------------------------------------------------------------
// 客户端发送线程主处理函数
static void* ClientSendThreadMainProc( void* p_pSendThread )
{
	CClientSendThread* pSendThread = (CClientSendThread*)p_pSendThread;
	if( pSendThread == NULL )
		return NULL;

	while ( !pSendThread->IsExit() )
	{
		pSendThread->OnSendEvent();
	}
	return NULL;
}
//-------------------------------------------------------------------------
// CClientSendThread
//-------------------------------------------------------------------------
CClientSendThread::CClientSendThread()
	: m_bIsExit( false )
	, m_bEncrypt( false )
	, m_nSocketID( 0 )
	, m_pSemphore( NULL )
	, m_bIsHadThread( false )
	, m_pThreadHandle( NULL )
{
	memset( m_szTmpSendBuf, 0, MAX_SENDBUF_LEN + 2 );
	
	// 创建信号量
	SEM_CREATE(*m_pSemphore,0);
}
//-------------------------------------------------------------------------
CClientSendThread::~CClientSendThread()
{
	Stop();

	// 销毁信号量
	SEM_CLOSE(*m_pSemphore);

	m_pSemphore = NULL;
}
//-------------------------------------------------------------------------
// 启动
bool CClientSendThread::Start( int p_nSocketID )
{
	if( p_nSocketID == 0 || p_nSocketID == INVALID_SOCKET )
	{
		return false;
	}
	m_nSocketID = p_nSocketID;

	// 创建线程
	m_bIsExit = false;
	m_pThreadHandle = new thread( ClientSendThreadMainProc, (void*)this );
	if( m_pThreadHandle == NULL )
		return false;
	m_bIsHadThread = true;

	return true;
}
//-------------------------------------------------------------------------
// 停止
void CClientSendThread::Stop()
{
	if( m_bIsExit )
		return;

	m_bIsExit = true;

	SEM_SIGNAL( m_pSemphore );

	if( m_bIsHadThread )
	{
		m_pThreadHandle->join();
		m_pThreadHandle->detach();
		// 是否要delete还值得商榷
		delete m_pThreadHandle;
		m_pThreadHandle = NULL;
	}

	// 清除发送队列
	m_WaitingSendList.GetOutList().clear();
	m_WaitingSendList.SwitchState();
	m_WaitingSendList.GetOutList().clear();

	m_nSocketID = INVALID_SOCKET;
}
//-------------------------------------------------------------------------
// 请求发送
bool CClientSendThread::PushSend( const char* p_szBuf, int p_nLen )
{
	if( p_szBuf == NULL || p_nLen <= 0 || p_nLen > MAX_SENDBUF_LEN )
		return false;

	SSendDataPack tagPack;
	tagPack.m_dwLen = p_nLen;
	tagPack.m_vecData.resize( p_nLen );
	if( static_cast<int>(tagPack.m_vecData.capacity()) < p_nLen )
	{
		return false;
	}
	memcpy( &tagPack.m_vecData[0], p_szBuf, p_nLen );
	m_WaitingSendList.Push( &tagPack );

	SEM_SIGNAL( m_pSemphore );
	return true;
}
//-------------------------------------------------------------------------
// 收到发送事件
void CClientSendThread::OnSendEvent()
{
	int nSemWaitRet = SEM_WAIT( m_pSemphore );
	if( nSemWaitRet < 0 )
		return;

	m_WaitingSendList.SwitchState();
	while( !m_WaitingSendList.GetOutList().empty() )
	{
		SSendDataPack& tagPacket = m_WaitingSendList.GetOutList().front();
		OnSend( &tagPacket.m_vecData[0], tagPacket.m_dwLen );
		m_WaitingSendList.GetOutList().pop_front();
	}
}
//-------------------------------------------------------------------------
// 真实发送
void CClientSendThread::OnSend( const char* p_szBuf, int p_nLen )
{
	if( p_szBuf == NULL || p_nLen <= 0 || p_nLen > MAX_SENDBUF_LEN )
		return;

	char* szSendBuf = NULL;
	int	nSendLen = sizeof(m_szTmpSendBuf);

	memcpy( m_szTmpSendBuf, p_szBuf, p_nLen );
	szSendBuf = m_szTmpSendBuf;
	nSendLen = p_nLen;

	int nResult = send_mesg( m_nSocketID, szSendBuf, nSendLen );

	if( nResult < 0 )
	{
		// error show
	}
}
//-------------------------------------------------------------------------
// 获取信号器
semaphore* CClientSendThread::GetSemphore()
{
	return m_pSemphore;
}
//-------------------------------------------------------------------------
// 判断是否结束退出
bool CClientSendThread::IsExit()
{
	return m_bIsExit;
}
//-------------------------------------------------------------------------