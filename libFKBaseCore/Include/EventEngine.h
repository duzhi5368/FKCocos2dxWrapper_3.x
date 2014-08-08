//*************************************************************************
//	创建日期:	2014-8-7   17:38
//	文件名称:	EventEngine.h
//  创 建 人:   王宏张 FreeKnight	
//	版权所有:	MIT
//	说    明:	
//*************************************************************************

#pragma once

//-------------------------------------------------------------------------
#include "../../FKCommon/FKCommonDef.h"
#include "../../FKCommon/IFKEventEngine.h"
#include "TEventEngine.h"
#include <list>
using std::list;
//-------------------------------------------------------------------------
struct OnVoteObject
{
	bool operator() ( IEventVoteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
		uint8 p_ucSrcID, const char* p_szContext, int p_nLen )
	{
		if( p_pSink == NULL )
			return false;

		return p_pSink->OnVote(p_ucEventID, p_ucSrcType, p_ucSrcID, p_szContext, p_nLen );
	}
};
//-------------------------------------------------------------------------
struct OnExecuteObject
{
	bool operator() ( IEventExecuteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
		uint8 p_ucSrcID, const char* p_szContext, int p_nLen )
	{
		if( p_pSink == NULL )
			return false;

		p_pSink->OnExecute( p_ucEventID, p_ucSrcType, p_ucSrcID, p_szContext, p_nLen );
		return true;
	}
};
//-------------------------------------------------------------------------
class CEventEngine : public IEventEngine
{
	typedef TEventEngine< IEventVoteSink, OnVoteObject >		CENTER_VOTE;
	typedef TEventEngine< IEventExecuteSink, OnExecuteObject >	CENTER_EXECUTE;
public:
	CEventEngine();
	virtual ~CEventEngine();

public:
		// 创建
	virtual bool	Create();
	// 释放
	virtual void	Release();
	
public:
	// 发送投票事件   
	virtual bool	FireVote( uint8 p_ucEventID, uint8 p_ucSrcType, uint32 p_unSrcID,
		const char* p_szContext, int p_nLen );
	// 发送执行事件
	virtual void	FireExecute( uint8 p_ucEventID, uint8 p_ucSrcType, uint32 p_unSrcID,
		const char* p_szContext, int p_nLen );

	/** 订阅投票事件
	@param   p_pSink		: 回调sink
	@param   p_ucEventID	: 事件ID
	@param   p_ucSrcType	：发送源类型
	@param	 p_unSrcID		: 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 p_szDesc		: 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false 
	*/
	virtual bool	Subscibe( IEventVoteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
		uint32 p_unSrcID, const char* p_szDesc );

	/** 订阅执行事件
	@param   p_pSink		: 回调sink
	@param   p_ucEventID	: 事件ID
	@param   p_ucSrcType	：发送源类型
	@param	 p_unSrcID		: 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 p_szDesc		: 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false 
	*/
	virtual bool	Subscibe( IEventExecuteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
		uint32 p_unSrcID, const char* p_szDesc );

	/** 取消订阅投票事件
	@param   p_pSink		: 回调sink
	@param   p_ucEventID	: 事件ID
	@param   p_ucSrcType	：发送源类型
	@param	 p_unSrcID		: 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@return  成功返回true,失败返回false 
	*/
	virtual bool	UnSubscibe( IEventVoteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
		uint32 p_unSrcID );

	/** 取消订阅执行事件
	@param   p_pSink		: 回调sink
	@param   p_ucEventID	: 事件ID
	@param   p_ucSrcType	：发送源类型
	@param	 p_unSrcID		: 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@return  成功返回true,失败返回false 
	*/
	virtual bool	UnSubscibe( IEventExecuteSink* p_pSink, uint8 p_ucEventID, uint8 p_ucSrcType,
		uint32 p_unSrcID );

private:
	CENTER_VOTE		m_VoteCenter;		// 投票中心
	CENTER_EXECUTE	m_ExecuteCenter;	// 执行中心
};
//-------------------------------------------------------------------------