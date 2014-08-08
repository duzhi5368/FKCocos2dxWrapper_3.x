//-------------------------------------------------------------------------
#include "../Include/EventEngine.h"
//-------------------------------------------------------------------------
CEventEngine::CEventEngine()
{

}
//-------------------------------------------------------------------------
CEventEngine::~CEventEngine()
{

}
//-------------------------------------------------------------------------
// 创建
bool CEventEngine::Create()
{
	return true;
}
//-------------------------------------------------------------------------
// 释放
void CEventEngine::Release()
{
	delete this;
}
//-------------------------------------------------------------------------
// 发送投票事件   
bool CEventEngine::FireVote( uint8 p_ucEventID, uint8 p_ucSrcType, uint32 p_unSrcID,
						 const char* p_szContext, int p_nLen )
{
	return m_VoteCenter.Fire( p_ucEventID, p_ucSrcType, p_unSrcID, p_szContext, p_nLen );
}
//-------------------------------------------------------------------------
// 发送执行事件
void CEventEngine::FireExecute( uint8 p_ucEventID, uint8 p_ucSrcType, uint32 p_unSrcID,
							const char* p_szContext, int p_nLen )
{
	m_ExecuteCenter.Fire( p_ucEventID, p_ucSrcType, p_unSrcID, p_szContext, p_nLen );
}
//-------------------------------------------------------------------------
bool CEventEngine::Subscibe( IEventVoteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
						 uint32 p_unSrcID, const char* p_szDesc )
{
	return m_VoteCenter.Subscibe( p_pSink, p_ucEventID, p_ucSrcType, p_unSrcID, p_szDesc );
}
//-------------------------------------------------------------------------
bool CEventEngine::Subscibe( IEventExecuteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
						 uint32 p_unSrcID, const char* p_szDesc )
{
	return m_ExecuteCenter.Subscibe( p_pSink, p_ucEventID, p_ucSrcType, p_unSrcID, p_szDesc );
}
//-------------------------------------------------------------------------
bool CEventEngine::UnSubscibe( IEventVoteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
						   uint32 p_unSrcID )
{
	return m_VoteCenter.UnSubscibe( p_pSink, p_ucEventID, p_ucSrcType, p_unSrcID );
}
//-------------------------------------------------------------------------

bool CEventEngine::UnSubscibe( IEventExecuteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
							uint32 p_unSrcID )
{
	return m_ExecuteCenter.UnSubscibe( p_pSink, p_ucEventID, p_ucSrcType, p_unSrcID );
}
//-------------------------------------------------------------------------